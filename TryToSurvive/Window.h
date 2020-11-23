#pragma once
#include <queue>
#include "Event.h"
#include "View.h"
#include "EventManager.h"
#include "Sprite.h"

#define TRANSPARENT_COLOR RGB(255, 0, 255)

class Window
{
public:
	Window(HWND l_hwnd);

	void Update();

	LRESULT ProcessEvents(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);
	View GetView() { return m_view; }
	void SetView(View l_view) { m_view = l_view; };
	sf::Vector2u GetWindowSize() { return m_windowSize; }
	sf::FloatRect GetViewSpace();
	EventManager* GetEventManager();
	void BeginDraw();
	void EndDraw();
	bool IsDone();
	bool IsFullscreen();
	bool IsFocused();
	View GetDefaultView();
	void SetWindow(HWND hwnd) { m_hwnd = hwnd; }
	void Draw(int destX, int destY, int heigth, int width, int srcX, int srcY, HBITMAP img);
	void Draw(Sprite sprite);
	HDC GetCompatibleDC() { return CreateCompatibleDC(m_hdc); }

private:
	ttsv::Event::Key GetKey(WPARAM key);
	void PushEvent(ttsv::Event event);
	bool PollEvent(ttsv::Event& event);
	EventManager m_eventManager;


	std::queue<ttsv::Event> m_events;
	sf::Vector2u m_windowSize;
	View m_view;

	HWND m_hwnd;
	HDC m_hdc;
	HDC m_tempDC;
	HDC m_bitmapDC;

	PAINTSTRUCT m_ps;
	HBITMAP m_hbmBack;
	HBITMAP m_tempBm;
	BITMAP bm;
	
	bool m_isDone;
	bool m_isFullscreen;
	bool m_isFocused;
	
};

