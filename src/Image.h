#pragma once

struct Pixel;

class Image
{
public:
	virtual ~Image();

	void init(int w, int h, int nChannels);
	void reset();
	bool isValid() const;
	bool load(const char* filename);
	bool write(const char* filename) const;
	void computeTileMean(Pixel& meanPixel, int tileStartX, int tileStartY, int tileSize) const;
	void computeTileMean(float& meanR, float& meanG, float& meanB, float& meanA, int tileStartX, int tileStartY, int tileSize) const;
	void computeTileMeans(Image& tileMeans, int tileSize) const;
	void replaceTile(const Image& tile, int tileStartX, int tileStartY);
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int getNumChannels() const { return channels; }
	int sizeInBytes() const;
	void readPixel(Pixel& pixel, int x, int y) const;
	void readPixel(float& r, float& g, float& b, float& a, int x, int y) const;
	void writePixel(const Pixel& pixel, int x, int y);
	void writePixel(float r, float g, float b, float a, int x, int y);
	void cropToSquare(Image& croppedImage, int w = 0, int h = 0) const;
	void resize(Image& resizedImage, int w, int h) const;

private:
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char* data = nullptr;
};
