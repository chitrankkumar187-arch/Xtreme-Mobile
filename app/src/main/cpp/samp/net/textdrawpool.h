#pragma once
#include "../main.h"
#include "../game/textdraw.h"

#define MAX_PLAYER_TEXT_DRAWS 256

class CPlayerTextDrawPool
{
public:
    CPlayerTextDrawPool();
    ~CPlayerTextDrawPool();

    void New(uint16_t id, TEXT_DRAW_TRANSMIT* data, const char* text);
    void Delete(uint16_t id);
    void Draw();
    void SetText(uint16_t id, const char* text);
    void SetSelectState(bool state, uint32_t color = 0);

    CTextDraw* GetAt(uint16_t id)
    {
        if (id >= MAX_PLAYER_TEXT_DRAWS) return nullptr;
        if (!m_bSlotState[id]) return nullptr;
        return m_pTextDraw[id];
    }

private:
    CTextDraw* m_pTextDraw[MAX_PLAYER_TEXT_DRAWS];
    bool m_bSlotState[MAX_PLAYER_TEXT_DRAWS];
    bool m_bSelectState;
    uint32_t m_dwHoverColor;
    uint16_t m_wClickedTextDrawID;
};
