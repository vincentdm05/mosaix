#pragma once

class Image
{
public:
	virtual ~Image();

	bool load(const char* filename);
	bool write(const char* filename);
	void computeTileMeans(int tileSize);
	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char* data = nullptr;

	void readPixel(float& r, float& g, float& b, float& a, int x, int y) const;
	void writePixel(float r, float g, float b, float a, int x, int y);
};
