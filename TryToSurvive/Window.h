#pragma once
#include <queue>
#include "Event.h"
#include "View.h"
#include "EventManager.h"

class Window
{
public:
	Window();

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
	bool m_isDone;
	bool m_isFullscreen;
	bool m_isFocused;
	
};

