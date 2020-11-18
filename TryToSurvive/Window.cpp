#include <Windows.h>
#include <WinUser.h>
#include "Window.h"
#include "Event.h"

Window::Window()
{
   
}



LRESULT Window::ProcessEvents(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    bool isUp;
	ttsv::Event event;

	switch (message)
	{
	case WM_QUIT:
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		event.type = ttsv::Event::KeyPressed;
		event.key.code = GetKey(wParam);
		PushEvent(event);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
   
}



ttsv::Event::Key Window::GetKey(WPARAM key)
{
	switch (key)
	{
	case VK_LEFT:       return ttsv::Event::Key::Left;
	case VK_RIGHT:      return ttsv::Event::Key::Right;
	case VK_UP:         return ttsv::Event::Key::Up;
	case VK_DOWN:       return ttsv::Event::Key::Down;
	case 'A':			return ttsv::Event::Key::A;
	case 'W':           return ttsv::Event::Key::W;
	case 'S':           return ttsv::Event::Key::S;
	case 'D':           return ttsv::Event::Key::D;
	case VK_SPACE:      return ttsv::Event::Key::Space;
	case VK_RETURN:     return ttsv::Event::Key::Enter;
	default:
		return ttsv::Event::Key();
	}
	
}

void Window::PushEvent(ttsv::Event event)
{
	m_events.push(event);
}
