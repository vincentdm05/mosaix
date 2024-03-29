#include <Image.h>

#include <Pixel.h>

#include <exif.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cassert>
#include <fstream>
#include <iostream>

#define MAX_CHANNELS 4
// Max to avoid overflow with 4 channels
#define MAX_SIDE_LENGTH 16384

Image::~Image()
{
	delete data;
}

void Image::init(int w, int h, int nChannels)
{
	assert(w > 0);
	assert(h > 0);
	assert(w < MAX_SIDE_LENGTH);
	assert(h < MAX_SIDE_LENGTH);
	assert(nChannels > 0);
	assert(nChannels <= MAX_CHANNELS);

	if (data)
		reset();

	width = w;
	height = h;
	channels = nChannels;
	const int dataSize = sizeInBytes();
	data = new unsigned char[dataSize];
}

void Image::reset()
{
	width = 0;
	height = 0;
	channels = 0;
	delete data;
	data = nullptr;
}

bool Image::isValid() const
{
	return width > 0 &&
		width < MAX_SIDE_LENGTH &&
		height > 0 &&
		height < MAX_SIDE_LENGTH &&
		channels > 0 &&
		channels < MAX_CHANNELS &&
		data != nullptr;
}

bool Image::load(const char* filename)
{
	if (data)
		reset();

	data = stbi_load(filename, &width, &height, &channels, 0);

	if (data == nullptr)
	{
		std::cerr << "Could not load image '" << filename << "'." << std::endl;
		return false;
	}

	// Rotate data to get proper orientation
	const int orientation = getOrientationFromExif(filename);
	const int rotation = getRotationFromOrientation(orientation);
	rotate(rotation);

	std::cout << "Successfully loaded image '" << filename <<
		"' with size " << width << "x" << height <<
		" and " << channels << " channels." << std::endl;
	if (rotation)
	{
		std::cout << "Rotated by " << (rotation * 90) << " degrees." << std::endl;
	}

	return true;
}

bool Image::write(const char* filename) const
{
	if (0 == stbi_write_png(filename, width, height, channels, (const void*)data, 0))
	{
		std::cerr << "Could not write image '" << filename << "'." << std::endl;
		return false;
	}

	std::cout << "Successfully wrote image '" << filename << "'." << std::endl;

	return true;
}

void Image::computeTileMean(Pixel& meanPixel, int tileStartX, int tileStartY, int tileSize) const
{
	computeTileMean(meanPixel.r, meanPixel.g, meanPixel.b, meanPixel.a, tileStartX, tileStartY, tileSize);
}

void Image::computeTileMean(float& meanR, float& meanG, float& meanB, float& meanA, int tileStartX, int tileStartY, int tileSize) const
{
	int numSamples = 0;
	meanR = 0.0f;
	meanG = 0.0f;
	meanB = 0.0f;
	meanA = 0.0f;

	for (int indexInTile = 0; indexInTile < tileSize * tileSize; indexInTile++)
	{
		const int y = tileStartY + indexInTile / tileSize;
		const int x = tileStartX + indexInTile % tileSize;

		if (x < width && y < height)
		{
			float r, g, b, a;
			readPixel(r, g, b, a, x, y);
			meanR += r;
			meanG += g;
			meanB += b;
			meanA += a;

			numSamples++;
		}
	}

	if (numSamples)
	{
		const float normalizationFactor = 1.0f / float(numSamples);
		meanR *= normalizationFactor;
		meanG *= normalizationFactor;
		meanB *= normalizationFactor;
		meanA *= normalizationFactor;
	}
}

void Image::computeTileMeans(Image& tileMeans, int tileSize) const
{
	const int numTilesX = (width + tileSize - 1) / tileSize;
	const int numTilesY = (height + tileSize - 1) / tileSize;

	tileMeans.init(numTilesX, numTilesY, channels);

	for (int tileY = 0; tileY < numTilesY; tileY++)
	{
		const int tileStartY = tileY * tileSize;

		for (int tileX = 0; tileX < numTilesX; tileX++)
		{
			const int tileStartX = tileX * tileSize;

			float meanR, meanG, meanB, meanA;
			computeTileMean(meanR, meanG, meanB, meanA, tileStartX, tileStartY, tileSize);

			tileMeans.writePixel(meanR, meanG, meanB, meanA, tileX, tileY);
		}
	}
}

void Image::replaceTile(const Image& tile, int tileStartX, int tileStartY)
{
	assert(tileStartX < width);
	assert(tileStartY < height);

	for (int tileY = 0; tileY < tile.getHeight(); tileY++)
	{
		const int y = tileStartY + tileY;
		for (int tileX = 0; tileX < tile.getWidth(); tileX++)
		{
			const int x = tileStartX + tileX;
			if (x < width && y < height)
			{
				Pixel pixel;
				tile.readPixel(pixel, tileX, tileY);
				writePixel(pixel, x, y);
			}
		}
	}
}

int Image::sizeInBytes() const
{
	return width * height * channels;
}

void Image::readPixel(Pixel& pixel, int x, int y) const
{
	readPixel(pixel.r, pixel.g, pixel.b, pixel.a, x, y);
}

void Image::readPixel(float& r, float& g, float& b, float& a, int x, int y) const
{
	readPixelInternal(r, g, b, a, data, getWidth(), getHeight(), getNumChannels(), x, y);
}

void Image::writePixel(const Pixel& pixel, int x, int y)
{
	writePixel(pixel.r, pixel.g, pixel.b, pixel.a, x, y);
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

void Image::cropToSquare(Image& croppedImage, int w, int h) const
{
	assert(w >= 0);
	assert(h >= 0);
	assert(w < MAX_SIDE_LENGTH);
	assert(h < MAX_SIDE_LENGTH);

	int smallestSide = getWidth();
	float s0 = 0.0f;
	float t0 = 0.0f;
	float s1 = 1.0f;
	float t1 = 1.0f;

	if (getWidth() > getHeight())
	{
		// Horizontal image
		smallestSide = getHeight();
		s0 = float(getWidth() - getHeight()) * 0.5f / float(getWidth());
		t0 = 0.0f;
		s1 = 1.0f - s0;
		t1 = 1.0f;
	}
	else if (getWidth() < getHeight())
	{
		// Vertical image
		smallestSide = getWidth();
		s0 = 0.0f;
		t0 = (float(getHeight() - getWidth()) * 0.5f / float(getHeight())) * 0.5f;
		s1 = 1.0f;
		t1 = t0 + float(getWidth()) / float(getHeight());
	}

	if (w == 0)
		w = smallestSide;
	if (h == 0)
		h = smallestSide;

	croppedImage.init(w, h, getNumChannels());

	int alphaChannel = getNumChannels() == 4 ? STBIR_FLAG_ALPHA_PREMULTIPLIED : STBIR_ALPHA_CHANNEL_NONE;

	stbir_resize_region(data, getWidth(), getHeight(), 0,
		croppedImage.data, croppedImage.getWidth(), croppedImage.getHeight(), 0,
		STBIR_TYPE_UINT8, getNumChannels(), alphaChannel, 0,
		STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
		STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
		STBIR_COLORSPACE_LINEAR, nullptr,
		s0, t0, s1, t1);
}

void Image::resize(Image& resizedImage, int w, int h) const
{
	resizedImage.init(w, h, channels);
	stbir_resize_uint8(data, getWidth(), getHeight(), 0, resizedImage.data, w, h, 0, channels);
}

void Image::readPixelInternal(float& r, float& g, float& b, float& a,
	const unsigned char* source, int width, int height, int nChannels, int x, int y)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(source != nullptr);
	assert(x < width);
	assert(y < height);
	assert(nChannels > 0);
	assert(nChannels < MAX_CHANNELS);

	const uint8_t* p = &source[(y * width + x) * nChannels];
	r = float(*(p + 0)) / 255.0f;
	if (nChannels > 1)
		g = float(*(p + 1)) / 255.0f;
	if (nChannels > 2)
		b = float(*(p + 2)) / 255.0f;
	if (nChannels > 3)
		a = float(*(p + 3)) / 255.0f;
}

int Image::getOrientationFromExif(const char* filename) const
{
	static const int unspecifiedOrientation = 0;

	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Could not open file " << filename << " for reading." << std::endl;
		return unspecifiedOrientation;
	}

	file.seekg(0, std::ios::end);
	const size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	unsigned char *buffer = new unsigned char[fileSize];
	file.read(reinterpret_cast<char*>(buffer), fileSize);
	const int bytesRead = file.gcount();
	file.close();
	if (bytesRead != fileSize)
	{
		std::cerr << "Cannot read file " << filename << "." << std::endl;
		delete[] buffer;
		return unspecifiedOrientation;
	}

	easyexif::EXIFInfo info;
	int parseResult = info.parseFrom(buffer, fileSize);
	delete[] buffer;
	if (parseResult != PARSE_EXIF_SUCCESS)
	{
		if (parseResult == PARSE_EXIF_ERROR_CORRUPT)
		{
			std::cerr << "Error parsing EXIF: corrupt data." << std::endl;
		}
		return unspecifiedOrientation;
	}

	return info.Orientation;
}

// Get necessary rotation to orient image back to normal
int Image::getRotationFromOrientation(int orientation) const
{
	// Normal orientation, or vertically mirrored
	if (orientation == 1 || orientation == 2)
	{
		// No rotation
		return 0;
	}
	// Oriented at a 270 degree angle
	if (orientation == 5 || orientation == 6)
	{
		// Rotate by remaining 90 degrees
		return 1;
	}
	// Oriented at a 180 degree angle
	if (orientation == 3 || orientation == 4)
	{
		// Rotate by 180 degrees
		return 2;
	}
	// Oriented at a 90 degree angle
	if (orientation == 7 || orientation == 8)
	{
		// Rotate by 270 degrees
		return 3;
	}
	return 0;
}

void Image::rotate(int ninetyDegreesRotateAmount)
{
	if (ninetyDegreesRotateAmount == 0)
		return;

	int savedWidth = getWidth();
	int savedHeight = getHeight();
	unsigned char* savedData = data;
	data = nullptr;

	if (ninetyDegreesRotateAmount == 1 || ninetyDegreesRotateAmount == 3)
	{
		init(getHeight(), getWidth(), getNumChannels());
	}
	else
	{
		init(getWidth(), getHeight(), getNumChannels());
	}

	Pixel pixel;
	for (int y = 0; y < getHeight(); y++)
	{
		for (int x = 0; x < getWidth(); x++)
		{
			int sourceX = x;
			int sourceY = y;
			if (ninetyDegreesRotateAmount == 1)
			{
				sourceX = y;
				sourceY = savedHeight - 1 - x;
			}
			else if (ninetyDegreesRotateAmount == 2)
			{
				sourceX = savedWidth - 1 - x;
				sourceY = savedHeight - 1 - y;
			}
			else if (ninetyDegreesRotateAmount == 3)
			{
				sourceX = savedWidth - 1 - y;
				sourceY = x;
			}
			readPixelInternal(pixel.r, pixel.g, pixel.b, pixel.a,
				savedData, savedWidth, savedHeight, getNumChannels(), sourceX, sourceY);
			writePixel(pixel, x, y);
		}
	}

	delete savedData;
}
