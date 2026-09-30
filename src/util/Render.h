#pragma once
#include "Config.h"

namespace util
{
	inline CRGBA SetupColor(short color, bool friendly, const struct Config& cfg)
	{
		if (color < 0)
			return cfg.GPS_LINE_CLR;

		if (cfg.ENABLE_CUSTOM_CLRS)
		{
			unsigned int lookupColor = static_cast<unsigned int>(color);
			if (color == 7) // THREAT: blue if friendly, red otherwise
				lookupColor = friendly ? 2u : 0u;
			else if (color == 8 && !cfg.CUSTOM_COLORS.contains(8)) // DESTINATION defaults to yellow
				lookupColor = 4u;

			auto it = cfg.CUSTOM_COLORS.find(lookupColor);
			return (it != cfg.CUSTOM_COLORS.end()) ? it->second : cfg.GPS_LINE_CLR;
		}

		if (color > 8)
			return cfg.GPS_LINE_CLR;

		return CRadar::GetRadarTraceColour(color, 1, friendly);
	}

	constexpr void Setup2dVertex(RwIm2DVertex &vertex, const double x, const double y, const CRGBA &clr)
	{
		vertex.x = x;
		vertex.y = y;
		vertex.u = vertex.v = 0.0f;
		vertex.z = CSprite2d::NearScreenZ + 0.0001f;
		vertex.rhw = CSprite2d::RecipNearClip;

		vertex.emissiveColor = RWRGBALONG(clr.r, clr.g, clr.b, clr.a);
	}
} // namespace util