#pragma once
#include <Windows.h>
#include <vector>

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


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

		bool intersects(const FloatRect l_rect) {
			bool isLeftInX = (l_rect.left > left && l_rect.left < left + width);
			bool isTopInY = l_rect.top > top && l_rect.top < top + height;
			bool isRightInX = l_rect.left + l_rect.width > left && l_rect.left + width < left + width;
			bool isBottomInY = l_rect.top + height > top && l_rect.top + height < top + height;
			return (isLeftInX && isTopInY) || (isRightInX && isTopInY) || (isLeftInX && isBottomInY) || (isRightInX && isBottomInY);
		}

		bool intersects(const FloatRect& rectangle, FloatRect& intersection) const {
			float r1MinX = min(left, left + width);
			float r1MaxX = max(left, left + width);
			float r1MinY = min(top, (top + height));
			float r1MaxY = max(top, (top + height));

			float r2MinX = min(rectangle.left, (rectangle.left + rectangle.width));
			float r2MaxX = max(rectangle.left, (rectangle.left + rectangle.width));
			float r2MinY = min(rectangle.top, (rectangle.top + rectangle.height));
			float r2MaxY = max(rectangle.top, (rectangle.top + rectangle.height));

			float interLeft = max(r1MinX, r2MinX);
			float interTop = max(r1MinY, r2MinY);
			float interRight = min(r1MaxX, r2MaxX);
			float interBottom = min(r1MaxY, r2MaxY);

			if ((interLeft < interRight) && (interTop < interBottom))
			{
				intersection = FloatRect(Vector2f(interLeft, interTop), Vector2f(interRight - interLeft, interBottom - interTop));
				return true;
			}
			else
			{
				intersection = FloatRect(Vector2f(0, 0), Vector2f(0, 0));
				return false;
			}
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