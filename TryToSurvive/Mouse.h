#pragma once
#include "Event.h"

class Mouse {
public:
	static bool IsButtonPressed(ttsv::Event::Button button) {
        int vkey = 0;
        switch (button)
        {
        case ttsv::Event::btnLeft:     vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON; break;
        case ttsv::Event::btnRight:    vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON; break;
        case ttsv::Event::btnMiddle:   vkey = VK_MBUTTON;  break;
        default:              vkey = 0;           break;
        }
        return (GetAsyncKeyState(vkey) & 0x8000) != 0;
	}
};