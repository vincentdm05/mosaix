#pragma once

class Image
{
public:
	virtual ~Image();

	void init(int w, int h, int nChannels);
	void reset();
	bool load(const char* filename);
	bool write(const char* filename);
	void computeTileMeans(Image& tileMeans, int tileSize);
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int getNumChannels() const { return channels; }
	int sizeInBytes() const;
	void readPixel(float& r, float& g, float& b, float& a, int x, int y) const;
	void writePixel(float r, float g, float b, float a, int x, int y);
	void cropToSquare(Image& croppedImage, int w = 0, int h = 0);
	void resize(Image& resizedImage, int w, int h);

private:
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char* data = nullptr;
};
