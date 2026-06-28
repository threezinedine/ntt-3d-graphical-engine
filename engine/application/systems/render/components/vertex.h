#pragma once

#include "services.h"

namespace ntt {

typedef Matrix<f32, 4, 1> Color;

struct Vertex
{
	Vec3f position;
	Vec2f texCoord;
	Color color;

	Vertex(const Vec3f& pos, const Vec2f& tex, const Color& col)
		: position(pos)
		, texCoord(tex)
		, color(col)
	{
	}
};

} // namespace ntt
