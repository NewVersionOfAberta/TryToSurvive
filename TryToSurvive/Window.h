#pragma once
#include <queue>
#include "Event.h"

class Window
{
public:
	Window();

	LRESULT ProcessEvents(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);
	BOOL IsKeyPressed(ttsv::Event::Key key);

private:
	ttsv::Event::Key GetKey(WPARAM key);
	void PushEvent(ttsv::Event event);

	std::queue<ttsv::Event> m_events;
};

