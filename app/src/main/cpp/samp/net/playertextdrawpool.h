#pragma once

#include <stdint.h>
#include "../game/textdraw.h"

#define MAX_PLAYER_TEXT_DRAWS 256

class CPlayerTextDrawPool
{
public:
    CPlayerTextDrawPool();
    ~CPlayerTextDrawPool();

    void New(uint16_t wTextDrawID, TEXT_DRAW_TRANSMIT* pTextDrawTransmit, const char* szText);
    void Delete(uint16_t wTextDrawID);
    void Draw();
    void SnapshotProcess();
    void SetSelectState(bool bState, uint32_t dwColor = 0);
    void SetText(uint16_t wTextDrawID, const char* szText);

    CTextDraw* GetAt(uint16_t wTextDrawID)
    {
        if (wTextDrawID >= MAX_PLAYER_TEXT_DRAWS) return nullptr;
        if (!m_bSlotState[wTextDrawID]) return nullptr;
        return m_pTextDraw[wTextDrawID];
    }

private:
    CTextDraw* m_pTextDraw[MAX_PLAYER_TEXT_DRAWS];
    bool m_bSlotState[MAX_PLAYER_TEXT_DRAWS];

    bool m_bSelectState;
    uint32_t m_dwHoverColor;
    uint16_t m_wClickedTextDrawID;
};
