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
    {
        Delete(i);
    }
}

void CPlayerTextDrawPool::New(uint16_t wTextDrawID, TEXT_DRAW_TRANSMIT* pTextDrawTransmit, const char* szText)
{
    if (wTextDrawID >= MAX_PLAYER_TEXT_DRAWS || !pTextDrawTransmit || !szText) return;

    if (m_pTextDraw[wTextDrawID])
        Delete(wTextDrawID);

    CTextDraw* pTextDraw = new CTextDraw(pTextDrawTransmit, szText);
    if (!pTextDraw) return;

    m_pTextDraw[wTextDrawID] = pTextDraw;
    m_bSlotState[wTextDrawID] = true;
}

void CPlayerTextDrawPool::Delete(uint16_t wTextDrawID)
{
    if (wTextDrawID >= MAX_PLAYER_TEXT_DRAWS) return;

    if (m_pTextDraw[wTextDrawID])
    {
        delete m_pTextDraw[wTextDrawID];
        m_pTextDraw[wTextDrawID] = nullptr;
    }

    m_bSlotState[wTextDrawID] = false;
}

void CPlayerTextDrawPool::Draw()
{
    for (int i = 0; i < MAX_PLAYER_TEXT_DRAWS; i++)
    {
        if (m_bSlotState[i] && m_pTextDraw[i])
        {
            m_pTextDraw[i]->Draw();
        }
    }
}

void CPlayerTextDrawPool::SetText(uint16_t wTextDrawID, const char* szText)
{
    if (wTextDrawID >= MAX_PLAYER_TEXT_DRAWS || !szText) return;

    CTextDraw* pTextDraw = GetAt(wTextDrawID);
    if (pTextDraw) pTextDraw->SetText(szText);
}

void CPlayerTextDrawPool::SetSelectState(bool bState, uint32_t dwColor)
{
    m_bSelectState = bState;
    m_dwHoverColor = dwColor;
}

void CPlayerTextDrawPool::SendClick()
{
    // placeholder for player-textdraw click RPC once the RPC name/id is wired
}
