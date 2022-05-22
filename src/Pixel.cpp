#include <Pixel.h>

float Pixel::dist(const Pixel& other) const
{
	const float dr = r - other.r;
	const float dg = g - other.g;
	const float db = b - other.b;
	const float da = a - other.a;
	return dr * dr + dg * dg + db * db + da * da;
}
