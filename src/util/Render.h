#pragma once
#include <cmath>
#include "Config.h"

namespace util
{
	// Deterministically derives a distinct, readable color for a blip color
	// index that has no configured override. Hues are spread using the
	// golden ratio conjugate so consecutive indices don't look alike, with
	// fixed saturation/value so nothing comes out too dark or washed out.
	// This means a third-party mod's own blip colors still look reasonable
	// on the GPS line even if its ini never configures them.
	inline CRGBA HashColorForIndex(unsigned int index)
	{
		constexpr double GOLDEN_RATIO_CONJUGATE = 0.6180339887498949;
		double hue = std::fmod(index * GOLDEN_RATIO_CONJUGATE, 1.0) * 6.0;
		constexpr double saturation = 0.75;
		constexpr double value = 0.95;

		int hi = static_cast<int>(hue);
		double f = hue - hi;
		double p = value * (1.0 - saturation);
		double q = value * (1.0 - saturation * f);
		double t = value * (1.0 - saturation * (1.0 - f));

		double r, g, b;
		switch (hi % 6)
		{
		case 0: r = value; g = t; b = p; break;
		case 1: r = q; g = value; b = p; break;
		case 2: r = p; g = value; b = t; break;
		case 3: r = p; g = q; b = value; break;
		case 4: r = t; g = p; b = value; break;
		default: r = value; g = p; b = q; break;
		}

		return CRGBA(static_cast<unsigned char>(r * 255.0), static_cast<unsigned char>(g * 255.0),
					static_cast<unsigned char>(b * 255.0), 255);
	}

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
			if (it != cfg.CUSTOM_COLORS.end())
				return it->second;

			// Outside vanilla's 0-8 range with no configured override -
			// most likely a third-party mod's own blip color. Give it a
			// distinct generated color instead of collapsing every
			// unconfigured mod color into the same default line color.
			return (lookupColor > 8) ? HashColorForIndex(lookupColor) : cfg.GPS_LINE_CLR;
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