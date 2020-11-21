#pragma once
#include <vector>
#include <Windows.h>


namespace sf {
	using Time = INT64;
	using Vector2i = std::pair<INT32, INT32>; // where first = x, second = y
	using Vector2f = std::pair<float, float>; // where first = x, second = y
	using Vector2u = std::pair<UINT32, UINT32>; // where first = x, second = y
	using Uint32 = UINT32;

	
	struct FloatRect {
		FloatRect(Vector2f topLeft, Vector2f size) {
			left = topLeft.first;
			top = topLeft.second;
			width = size.first;
			height = size.second;
		}

		float left;
		float top;
		float width;
		float height;
	};

	struct IntRect {
		IntRect(int l_x, int l_y, int l_height, int l_width) :
			x(l_x), y(l_y), heigth(l_height), width(l_width){}

		int x;
		int y;
		int width;
		int heigth;
	};

}