#include "BuildingRemoval.h"
#include "game/Entity/Building.h"
#include "game/Entity/Dummy.h"
#include "game/Entity/Object.h"
#include "game/COcclusion.h"
#include "Pools.h"
#include "util/CUtil.h"
#include <cmath>

void CBuildingRemoval::RemoveBuildingByPtr(CEntityGTA* pEntity)
{
    if (!pEntity)
        return;

    Log(
        "[REMOVE] REMOVING entity model=%d pos=%f,%f,%f",
        pEntity->m_nModelIndex,
        pEntity->GetPosition().x,
        pEntity->GetPosition().y,
        pEntity->GetPosition().z
    );

    CVector newPos = pEntity->GetPosition();
    newPos.z -= 2000.0f;

    pEntity->SetPosn(newPos);

    pEntity->m_bRemoveFromWorld = true;
    pEntity->m_nAreaCode = AREA_CODE_1;
    pEntity->m_bUsesCollision = false;

    if (pEntity->m_matrix)
    {
        CVector& matrixPos =
            pEntity->m_matrix->GetPosition();

        matrixPos.z -= 2000.0f;
    }
}

bool CBuildingRemoval::IsEntityValidForRemoval(CEntityGTA* entity) {
    if (!entity) return false;

    // Check if already removed or not visible
    if (entity->m_bRemoveFromWorld || !entity->m_bIsVisible) {
        return false;
    }

    // VTable validation for additional safety (optional but recommended)
    auto vtable = *reinterpret_cast<uintptr_t*>(entity);
    vtable -= g_libGTASA;


    if (vtable == 0x81E628) return false;


    return true;
}

float CBuildingRemoval::GetDistanceBetween3DPoints(const CVector* point1, const CVector* point2) {
    if (!point1 || !point2) return NAN; // Return NAN or max float

    float dx = point1->x - point2->x;
    float dy = point1->y - point2->y;
    float dz = point1->z - point2->z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

void CBuildingRemoval::RemoveOccluders(const CVector& position, float radius) {
    for (int32_t i = 0; i < COcclusion::NumOccludersOnMap; i++) {
        COccluder& occluder = COcclusion::aOccluders[i];

        CVector occluderPos;
        occluderPos.x = (float)occluder.iCenterX * 0.25f;
        occluderPos.y = (float)occluder.iCenterY * 0.25f;
        occluderPos.z = (float)occluder.iCenterZ * 0.25f;

        if (GetDistanceBetween3DPoints(&position, &occluderPos) < radius) {
            occluder.iCenterX = 0;
            occluder.iCenterY = 0;
            occluder.iCenterZ = 0;

            occluder.iLength = 0;
            occluder.iWidth = 0;
            occluder.iHeight = 0;

            occluder.iRotX = 0;
            occluder.iRotY = 0;
            occluder.iRotZ = 0;
        }
    }
}

void CBuildingRemoval::AddRemoval(
    uint32_t modelId,
    const CVector& pos,
    float radius)
{
    if (radius <= 0.0f)
        return;

    // Prevent duplicate removal entries.
    for (int i = 0; i < m_TotalRemovedObjects; i++)
    {
        const RemoveBuildingInfo& existing = m_RemoveBuildings[i];

        if (existing.modelId != modelId)
            continue;

        if (std::fabs(existing.position.x - pos.x) > 0.01f)
            continue;

        if (std::fabs(existing.position.y - pos.y) > 0.01f)
            continue;

        if (std::fabs(existing.position.z - pos.z) > 0.01f)
            continue;

        if (std::fabs(existing.radius - radius) > 0.01f)
            continue;

        // Already registered.
        return;
    }

    if (m_TotalRemovedObjects >= MAX_REMOVALS)
    {
        Log(
            "[REMOVE] Removal list full, model=%u pos=%f,%f,%f radius=%f",
            modelId,
            pos.x,
            pos.y,
            pos.z,
            radius
        );
        return;
    }

    RemoveBuildingInfo& info =
        m_RemoveBuildings[m_TotalRemovedObjects];

    info.modelId = modelId;
    info.position = pos;
    info.radius = radius;

    m_TotalRemovedObjects++;

    Log(
        "[REMOVE] Added model=%u pos=%f,%f,%f radius=%f total=%d",
        modelId,
        pos.x,
        pos.y,
        pos.z,
        radius,
        m_TotalRemovedObjects
    );

    // Remove a building that is already loaded.
    ProcessRemoveBuilding(modelId, pos, radius);
}

void CBuildingRemoval::ProcessRemoveBuilding(uint32_t modelId, const CVector& pos, float radius) {
    // Remove occluders with larger radius for safety
    RemoveOccluders(pos, 500.0f);

    // Use template function for all pools
    RemoveBuildingsInPool(GetBuildingPool(), modelId, pos, radius);
    RemoveBuildingsInPool(GetDummyPool(), modelId, pos, radius);
    RemoveBuildingsInPool(GetObjectPoolGta(), modelId, pos, radius);
}

// Template implementation for pool processing
template <typename PoolT>
void CBuildingRemoval::RemoveBuildingsInPool(
    PoolT* pool,
    uint32_t uiModel,
    const CVector& pos,
    float radius)
{
    if (!pool)
    {
        Log("[REMOVE] Pool is NULL model=%u", uiModel);
        return;
    }

    int checked = 0;
    int matched = 0;

    for (int i = 0; i < pool->GetSize(); i++)
    {
        auto* entity = pool->GetAt(i);

        if (!entity)
            continue;

        checked++;

        if (!IsEntityValidForRemoval(entity))
            continue;

        if (entity->m_nModelIndex != uiModel &&
            uiModel != static_cast<uint32_t>(-1))
        {
            continue;
        }

        float distance =
            GetDistanceBetween3DPoints(
                &pos,
                &entity->GetPosition()
            );

        if (distance <= radius)
        {
            matched++;

            Log(
                "[REMOVE] MATCH model=%d distance=%f radius=%f",
                entity->m_nModelIndex,
                distance,
                radius
            );

            RemoveBuildingByPtr(entity);
        }
    }

    Log(
        "[REMOVE] Pool scan requestedModel=%u checked=%d matched=%d",
        uiModel,
        checked,
        matched
    );
}

// Explicit template instantiations
template void CBuildingRemoval::RemoveBuildingsInPool<CPool<CBuilding>>(CPool<CBuilding>*, uint32_t, const CVector&, float);
template void CBuildingRemoval::RemoveBuildingsInPool<CPool<CDummy>>(CPool<CDummy>*, uint32_t, const CVector&, float);
template void CBuildingRemoval::RemoveBuildingsInPool<CPool<CObjectGta>>(CPool<CObjectGta>*, uint32_t, const CVector&, float);
