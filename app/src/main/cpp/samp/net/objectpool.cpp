#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame *pGame;

// 0.3.7
CObjectPool::CObjectPool()
{
    for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
    {
        m_bObjectSlotState[ObjectID] = false;
        m_pObjects[ObjectID] = nullptr;
    }

    m_iObjectCount = 0;
}
// 0.3.7
CObjectPool::~CObjectPool()
{
    for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
    {
        Delete(ObjectID);
    }
}

bool CObjectPool::New(
    OBJECTID ObjectID,
    int iModel,
    CVector vecPos,
    CVector vecRot,
    float fDrawDistance)
{
    Log("[OBJECTPOOL] New id=%d model=%d pos=%f,%f,%f",
        ObjectID,
        iModel,
        vecPos.x,
        vecPos.y,
        vecPos.z);

    if (ObjectID >= MAX_OBJECTS)
    {
        Log("[OBJECTPOOL] INVALID ID %d", ObjectID);
        return false;
    }

    if (m_pObjects[ObjectID] != nullptr)
    {
        Log("[OBJECTPOOL] Deleting existing object id=%d", ObjectID);
        Delete(ObjectID);
    }

    Log("[OBJECTPOOL] Calling CGame::NewObject id=%d model=%d",
        ObjectID,
        iModel);

    m_pObjects[ObjectID] =
        pGame->NewObject(
            iModel,
            vecPos,
            vecRot,
            fDrawDistance
        );

    if (!m_pObjects[ObjectID])
    {
        Log("[OBJECTPOOL] pGame->NewObject FAILED id=%d model=%d",
            ObjectID,
            iModel);
        return false;
    }

    m_bObjectSlotState[ObjectID] = true;

    Log("[OBJECTPOOL] SUCCESS id=%d model=%d",
        ObjectID,
        iModel);

    return true;
}

bool CObjectPool::Delete(OBJECTID ObjectID)
{
    if (ObjectID < MAX_OBJECTS && m_bObjectSlotState[ObjectID])
    {
        CObject* pObject = m_pObjects[ObjectID];
        if (pObject)
        {
            delete m_pObjects[ObjectID];
            m_pObjects[ObjectID] = nullptr;
            m_bObjectSlotState[ObjectID] = false;
        }
    }

    return true;
}

void CObjectPool::Process()
{
    static uint32_t s_dwLastTick = 0;

    if (s_dwLastTick == 0) {
        s_dwLastTick = GetTickCount();
    }

    uint32_t dwThisTick = GetTickCount();
    float fElapsedTime = (dwThisTick - s_dwLastTick) / 1000.0f;

    for (OBJECTID i = 0; i < MAX_OBJECTS; i++)
    {
        if (m_bObjectSlotState[i]) {
            m_pObjects[i]->Process(fElapsedTime);
        }
    }

    s_dwLastTick = dwThisTick;
}

void CObjectPool::RenderCustomObjects()
{
    for (OBJECTID i = 0; i < MAX_OBJECTS; i++)
    {
        if (!m_bObjectSlotState[i])
            continue;

        if (!m_pObjects[i])
            continue;

        m_pObjects[i]->RenderCustom();
    }
}

CObject* CObjectPool::FindObjectFromGtaPtr(CPhysical* pGtaObject)
{
    for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
    {
        if (m_pObjects[ObjectID] && m_pObjects[ObjectID]->m_pEntity && m_pObjects[ObjectID]->m_pEntity == pGtaObject)
            return m_pObjects[ObjectID];
    }

    return nullptr;
}

OBJECTID CObjectPool::FindIDFromGtaPtr(CPhysical* pGtaObject)
{
    for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
    {
        if (m_pObjects[ObjectID] && m_pObjects[ObjectID]->m_pEntity && m_pObjects[ObjectID]->m_pEntity == pGtaObject)
            return ObjectID;
    }

    return INVALID_OBJECT_ID;
}

void CObjectPool::ProcessMaterialText()
{
    for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
    {
        if (m_pObjects[ObjectID] && m_bObjectSlotState[ObjectID] == true)
        {
            m_pObjects[ObjectID]->ProcessMaterialText();
        }
    }
}
