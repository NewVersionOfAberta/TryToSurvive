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
		FloatRect():left(0), top(0), width(0), height(0){}
		FloatRect(Vector2f topLeft, Vector2f size) {
			left = topLeft.first;
			top = topLeft.second;
			width = size.first;
			height = size.second;
		}

		bool intersects(FloatRect l_rect) {
			bool isLeftInX = (l_rect.left > left && l_rect.left < left + width);
			bool isTopInY = l_rect.top > top && l_rect.top < top + height;
			bool isRightInX = l_rect.left + l_rect.width > left && l_rect.left + width < left + width;
			bool isBottomInY = l_rect.top + height > top && l_rect.top + height < top + height;
			return (isLeftInX && isTopInY) || (isRightInX && isTopInY) || (isLeftInX && isBottomInY) || (isRightInX && isBottomInY);
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