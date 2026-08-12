#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "playertextdrawpool.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

CPlayerTextDrawPool::CPlayerTextDrawPool()
{
    for (int i = 0; i < MAX_PLAYER_TEXT_DRAWS; i++)
    {
        m_pTextDraw[i] = nullptr;
        m_bSlotState[i] = false;
    }

    m_bSelectState = false;
    m_dwHoverColor = 0;
    m_wClickedTextDrawID = 0xFFFF;
}

CPlayerTextDrawPool::~CPlayerTextDrawPool()
{
    for (int i = 0; i < MAX_PLAYER_TEXT_DRAWS; i++)
        Delete(i);
}

void CPlayerTextDrawPool::New(
    uint16_t id,
    TEXT_DRAW_TRANSMIT* data,
    const char* text)
{
    Log("[PTD] New id=%u text='%s' style=%u x=%f y=%f",
        id,
        text ? text : "",
        data ? data->byteStyle : 0,
        data ? data->fX : 0.0f,
        data ? data->fY : 0.0f);

    if (id >= MAX_PLAYER_TEXT_DRAWS || !data || !text)
    {
        Log("[PTD] New rejected");
        return;
    }

    if (m_pTextDraw[id])
        Delete(id);

    CTextDraw* td =
        new CTextDraw(data, text);

    if (!td)
    {
        Log("[PTD] new CTextDraw failed");
        return;
    }

    m_pTextDraw[id] = td;
    m_bSlotState[id] = true;

    Log("[PTD] New successful id=%u", id);
}
void CPlayerTextDrawPool::Delete(uint16_t id)
{
    if (id >= MAX_PLAYER_TEXT_DRAWS) return;

    if (m_pTextDraw[id])
    {
        delete m_pTextDraw[id];
        m_pTextDraw[id] = nullptr;
    }

    m_bSlotState[id] = false;
}

void CPlayerTextDrawPool::Draw()
{
    static bool logged = false;

    if (!logged)
    {
        Log("[PTD] Draw() running");
        logged = true;
    }

    for (int i = 0; i < MAX_PLAYER_TEXT_DRAWS; i++)
    {
        if (m_bSlotState[i] && m_pTextDraw[i])
        {
            m_pTextDraw[i]->Draw();
        }
    }
}

void CPlayerTextDrawPool::SetText(uint16_t id, const char* text)
{
    if (id >= MAX_PLAYER_TEXT_DRAWS || !text) return;
    if (auto* td = GetAt(id)) td->SetText(text);
}

void CPlayerTextDrawPool::SetSelectState(bool state, uint32_t color)
{
    m_bSelectState = state;
    m_dwHoverColor = color;
}
