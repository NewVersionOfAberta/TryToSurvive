#include <Windows.h>
#include <WinUser.h>
#include "Window.h"
#include "Event.h"
#include "Usings.h"

Window::Window()
{
   
}



void Window::Update()
{
	ttsv::Event event;

	while (PollEvent(event)) {
		if (event.type == ttsv::Event::LostFocus) { m_isFocused = false; m_eventManager.SetFocus(false); }
		else if (event.type == ttsv::Event::GainedFocus) { m_isFocused = true; m_eventManager.SetFocus(true); }
		m_eventManager.HandleEvent(event);
	}

	m_eventManager.Update();
}

void Window::BeginDraw()
{

}
void Window::EndDraw() { }

bool Window::IsDone() { return m_isDone; }
bool Window::IsFullscreen() { return m_isFullscreen; }
bool Window::IsFocused() { return m_isFocused; }

View Window::GetDefaultView()
{
	RECT clientRect;
	GetClientRect(m_hwnd, &clientRect);
	return View(sf::Vector2f(clientRect.right / 2.0, clientRect.bottom / 2.0), sf::Vector2f(clientRect.right, clientRect.bottom));
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


sf::FloatRect Window::GetViewSpace() {
	sf::Vector2f viewCenter(m_view.getCenter());
	sf::Vector2f viewSize(m_view.getSize());
	sf::Vector2f viewSizeHalf(viewSize.first / 2, viewSize.second / 2);
	viewCenter.first -= viewSizeHalf.first;
	viewCenter.second -= viewSizeHalf.second;
	sf::FloatRect viewSpace(viewCenter, viewSize);
	return viewSpace;
}

EventManager* Window::GetEventManager()
{
	return &m_eventManager;
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

bool Window::PollEvent(ttsv::Event& event)
{
	if (!m_events.empty()) 
	{
		event = m_events.front();
		m_events.pop();
		return true;
	}
	else 
	{
		return false;
	}
}

