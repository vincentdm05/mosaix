#include <Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cassert>
#include <iostream>

Image::~Image()
{
	delete data;
}

bool Image::load(const char* filename)
{
	data = stbi_load(filename, &width, &height, &channels, 0);

	if (data == nullptr)
	{
		std::cerr << "Could not load image '" << filename << "'." << std::endl;
		return false;
	}

	std::cout << "Successfully loaded image '" << filename <<
		"' with size " << width << "x" << height <<
		" and " << channels << " channels." << std::endl;

	return true;
}

bool Image::write(const char* filename)
{
	if (0 == stbi_write_png(filename, width, height, channels, (const void*)data, 0))
	{
		std::cerr << "Could not write image '" << filename << "'." << std::endl;
		return false;
	}

	std::cout << "Successfully wrote image '" << filename << "'." << std::endl;

	return true;
}

void Image::computeTileMeans(int tileSize)
{
	const int numTilesX = (width + tileSize - 1) / tileSize;
	const int numTilesY = (height + tileSize - 1) / tileSize;

	std::cout << "Image will have " << numTilesX << " by " << numTilesY << " tiles." << std::endl;

	for (int tileY = 0; tileY < height; tileY += tileSize)
	{
		for (int tileX = 0; tileX < width; tileX += tileSize)
		{
			float numSamples = 0.0f;
			float meanR = 0.0f;
			float meanG = 0.0f;
			float meanB = 0.0f;
			float meanA = 0.0f;
			for (int indexInTile = 0; indexInTile < tileSize * tileSize; indexInTile++)
			{
				const int y = tileY + indexInTile / tileSize;
				const int x = tileX + indexInTile % tileSize;

				if (x < width && y < height)
				{
					float r, g, b, a;
					readPixel(r, g, b, a, x, y);
					meanR += r;
					meanG += g;
					meanB += b;
					meanA += a;

					numSamples += 1.0f;
				}
			}

			if (numSamples)
			{
				const float normalizationFactor = 1.0f / numSamples;
				meanR *= normalizationFactor;
				meanG *= normalizationFactor;
				meanB *= normalizationFactor;
				meanA *= normalizationFactor;
			}

			for (int indexInTile = 0; indexInTile < tileSize * tileSize; indexInTile++)
			{
				const int y = tileY + indexInTile / tileSize;
				const int x = tileX + indexInTile % tileSize;

				if (x < width && y < height)
				{
					writePixel(meanR, meanG, meanB, meanA, x, y);
				}
			}
		}
	}
}

void Image::readPixel(float& r, float& g, float& b, float& a, int x, int y) const
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);

	const uint8_t* p = &data[(y * width + x) * channels];
	r = float(*(p + 0)) / 255.0f;
	if (channels > 1)
		g = float(*(p + 1)) / 255.0f;
	if (channels > 2)
		b = float(*(p + 2)) / 255.0f;
	if (channels > 3)
		a = float(*(p + 3)) / 255.0f;
}

void Image::writePixel(float r, float g, float b, float a, int x, int y)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	assert(r >= 0.0f);
	assert(r <= 1.0f);
	assert(g >= 0.0f);
	assert(g <= 1.0f);
	assert(b >= 0.0f);
	assert(b <= 1.0f);
	assert(a >= 0.0f);
	assert(a <= 1.0f);

	const int pixelIndex = (y * width + x) * channels;
	data[pixelIndex + 0] = uint8_t(r * 255.0f);
	if (channels > 1)
		data[pixelIndex + 1] = uint8_t(g * 255.0f);
	if (channels > 2)
		data[pixelIndex + 2] = uint8_t(b * 255.0f);
	if (channels > 3)
		data[pixelIndex + 3] = uint8_t(a * 255.0f);
}
