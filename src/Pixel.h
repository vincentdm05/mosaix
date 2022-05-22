#pragma once

struct Pixel
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;

	float dist(const Pixel& other) const;
};
