#pragma once

#include <Image.h>

#include <filesystem>
#include <map>

struct Pixel;

class Mosaic
{
public:
	virtual ~Mosaic();

	void resetTiles();
	bool isValid() const;
	void setTileSize(int size);
	void setScaling(float s);
	bool setSourceImage(const std::filesystem::path& imagePath);
	bool setTilesFolder(const std::filesystem::path& folderPath);
	bool makeMosaicImage(Image& mosaicImage) const;

private:
	int tileSize = 0;
	float scaling = 1.0f;
	Image sourceImage;
	Image meanImage;
	int numTileImages = 0;
	Image* tileImages = nullptr;
	Pixel* tileMeans = nullptr;

	static int getNumFilesInFolder(const std::filesystem::path& folderPath);
};
