#pragma once
#include "../main.h"
#include "../game/textdraw.h"


class CTextDrawPool
{
public:
    CTextDrawPool();
    ~CTextDrawPool();

    void New(uint16_t wTextDrawID, TEXT_DRAW_TRANSMIT* pTextDrawTransmit, const char* szText);
    void Delete(uint16_t wTextDrawID);
    void Draw();
    void SnapshotProcess();
    void DrawImage();

    CTextDraw* GetAt(uint16_t wTextDrawID) {
        if (wTextDrawID >= MAX_TEXT_DRAWS) return nullptr;
        if (!m_bSlotState[wTextDrawID]) return nullptr;
        return m_pTextDraw[wTextDrawID];
    }

    bool GetState() {
        return m_bSelectState;
    }

    void SetSelectState(bool bState, uint32_t dwColor = 0);
    bool onTouchEvent(int type, bool multi, int x, int y);


private:
    void SendClick();

private:
    uint8_t m_bSlotState[MAX_TEXT_DRAWS];
    CTextDraw* m_pTextDraw[MAX_TEXT_DRAWS];

    bool m_bSelectState;
    uint32_t m_dwHoverColor;
    uint16_t m_wClickedTextDrawID;
};
