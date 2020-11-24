#include <Windows.h>
#include <WinUser.h>
#include "Window.h"
#include "Event.h"
#include "Usings.h"

#pragma comment(lib, "msimg32")

Window::Window(HWND l_hwnd) : m_hwnd(l_hwnd), m_eventManager()
{
	RECT clientRect;
	//m_hdc = GetDC(m_hwnd);
	//m_tempDC = CreateCompatibleDC(m_hdc);
	GetClientRect(l_hwnd, &clientRect);
	m_view = GetDefaultView();
	m_windowSize.first = clientRect.right;
	m_windowSize.second = clientRect.bottom;
	m_hdc = GetDC(m_hwnd);
	m_tempDC = CreateCompatibleDC(m_hdc);
	m_bitmapDC = CreateCompatibleDC(m_hdc);
	m_hbmBack = CreateCompatibleBitmap(m_hdc, clientRect.right, clientRect.bottom);
	m_tempBm = (HBITMAP)SelectObject(m_tempDC, m_hbmBack);
}

Window::~Window()
{
	SelectObject(m_tempDC, m_tempBm);
	ReleaseDC(m_hwnd, m_tempDC);
	DeleteDC(m_hdc);
	DeleteDC(m_bitmapDC);
	DeleteObject(m_hbmBack);
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
	RECT clientRect;
	//m_hdc = GetDC(m_hwnd);
	//m_tempDC = CreateCompatibleDC(m_hdc);
	GetClientRect(m_hwnd, &clientRect);
	
}



void Window::EndDraw() 
{
	RECT rcClient;
	bool isUp;
	GetClientRect(m_hwnd, &rcClient);
	//FillRect(m_tempDC, &rcClient, (HBRUSH)(BLACK_BRUSH));
	
	BeginPaint(m_hwnd, &m_ps);
	if (!BitBlt(m_hdc, 0, 0, m_view.getSize().first, m_view.getSize().second, m_tempDC, 0, 0, SRCCOPY)) {
		isUp = false;
	}
	else {
		isUp = true;
	}
	EndPaint(m_hwnd, &m_ps);
	InvalidateRect(m_hwnd, &rcClient, false);
	//ReleaseDC(m_hwnd, m_hdc);
	//EndPaint(m_hwnd, &m_ps);
}

bool Window::IsDone() { return m_isDone; }
bool Window::IsFullscreen() { return m_isFullscreen; }
bool Window::IsFocused() { return m_isFocused; }

View Window::GetDefaultView()
{
	RECT clientRect;
	GetClientRect(m_hwnd, &clientRect);
	return View(sf::Vector2f(clientRect.right / 2.0, clientRect.bottom / 2.0), sf::Vector2f(clientRect.right, clientRect.bottom));
}

void Window::Draw(int destX, int destY,  int heigth, int width, int srcX, int srcY, HBITMAP img)
{
	
	/*HDC hdc;
	if (!(hdc = CreateCompatibleDC(m_hdc))) 
	{ 
		DWORD a = GetLastError();
		return; 
	};*/
	HBITMAP temp;
	if (!GetObject(img, sizeof(BITMAP), &bm)) { return; }
	bool validated = true;
	if (!(temp = (HBITMAP)SelectObject(m_bitmapDC, img)))
	{ 
		return;
	};
	if (!TransparentBlt(m_tempDC, destX, destY, heigth, width, m_bitmapDC, srcX, srcY, heigth, width, TRANSPARENT_COLOR)) {
		validated = false;
	}
	SelectObject(m_bitmapDC, temp);
	//DeleteObject(img);
	//ReleaseDC(NULL, hdc);
	//DeleteObject(hdc);
}

void Window::Draw(Sprite sprite)
{
	sf::IntRect tmpTextureRect = sprite.GetTextureRect();
	Draw(sprite.GetPosition().first - m_view.getCenter().first + m_view.getSize().first / 2,
		sprite.GetPosition().second - m_view.getCenter().second + m_view.getSize().second / 2,
		tmpTextureRect.heigth, 
		tmpTextureRect.width,
		tmpTextureRect.x,
		tmpTextureRect.y,
		sprite.GetTexture());
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
		break;
	case WM_PAINT:
		if (this && m_tempDC && m_hdc)
		{
			
		}
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

