#include "RectExtensions.h"

#include <cmath>

namespace RectangleExtensions
{
	Vector2 RectangleExtensions::GetInersectionDepth(S2D::Rect* rectA, S2D::Rect* rectB)
	{
		float halfWidthA = rectA->Width / 2.0f;
		float halfHeightA = rectA->Height / 2.0f;
		float halfHeightB = rectB->Width / 2.0f;
		float halfWidthB = rectB->Height / 2.0f;

		Vector2* CenterA = new Vector2(rectA->Left() + halfWidthA, rectA->Top() + halfHeightA);
		Vector2* CenterB = new Vector2(rectB->Left() + halfWidthB, rectB->Top() + halfHeightB);

		float distanceX = CenterA->X - CenterB->X;
		float distanceY = CenterA->Y - CenterB->Y;
		float minDistanceX = halfWidthA + halfWidthB;
		float minDistanceY = halfHeightA + halfHeightB;

		if (abs(distanceX) >= minDistanceX || abs(distanceY) >= minDistanceY)
			return *Vector2::Zero;

		float depthX = distanceX > 0 ? minDistanceX - distanceX : -minDistanceX - distanceX;
		float depthY = distanceY > 0 ? minDistanceY - distanceY : -minDistanceY - distanceY;
		return Vector2(depthX, depthY);
	}
};