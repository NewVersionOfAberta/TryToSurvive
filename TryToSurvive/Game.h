#pragma once
#include <Windows.h>

class Game
{
public:
	Game();
	~Game();
	void HandleInput();
	void Update();
	void Render();
	//HDC GetWindow();
	//void SetHDC(HDC hdc);

private:
	HWND m_hwnd;
	
};

