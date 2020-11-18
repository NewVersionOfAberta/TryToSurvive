#pragma once
#include "Event.h"

class Keyboard
{
    public:

        static bool IsKeyPressed(ttsv::Event::Key key)
        {
            int vkey = 0;
            switch (key)
            {
            default:					        vkey = 0;             break;
            case	ttsv::Event::Key::Left:	    vkey = VK_LEFT;       break;
            case    ttsv::Event::Key::Right:	vkey = VK_RIGHT;      break;
            case	ttsv::Event::Key::Up:		vkey = VK_UP;         break;
            case	ttsv::Event::Key::Down:	    vkey = VK_DOWN;       break;
            case	ttsv::Event::Key::A:		vkey = 'A';           break;
            case	ttsv::Event::Key::W:		vkey = 'W';           break;
            case	ttsv::Event::Key::S:		vkey = 'S';           break;
            case	ttsv::Event::Key::D:		vkey = 'D';           break;
            case	ttsv::Event::Key::Space:	vkey = VK_SPACE;      break;
            case	ttsv::Event::Key::Enter:	vkey = VK_RETURN;     break;
            }
            return (GetAsyncKeyState(vkey) & 0x8000) != 0;
        }

    private:

    };

    