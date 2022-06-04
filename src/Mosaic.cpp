#include <Mosaic.h>

#include <Pixel.h>

#include <cassert>
#include <iterator>
#include <limits>

Mosaic::~Mosaic()
{
	delete[] tileImages;
}

void Mosaic::resetTiles()
{
	numTileImages = 0;
	delete[] tileImages;
	tileImages = nullptr;
	delete[] tileMeans;
	tileMeans = nullptr;
}

bool Mosaic::isValid() const
{
	return tileSize > 0 &&
		sourceImage.isValid() &&
		meanImage.isValid() &&
		numTileImages > 0 &&
		tileImages != nullptr &&
		tileMeans != nullptr;
}

void Mosaic::setTileSize(int size)
{
	assert(size > 0);
	assert(size <= 4096);

	tileSize = size;
}

void Mosaic::setScaling(float s)
{
	assert(s > 0.01f);
	assert(s <= 10.0f);

	scaling = s;
}

bool Mosaic::setSourceImage(const std::filesystem::path& imagePath)
{
	if (!sourceImage.load(imagePath.c_str()) || !sourceImage.isValid())
		return false;

	sourceImage.computeTileMeans(meanImage, int(tileSize / scaling));

	return true;
}

bool Mosaic::setTilesFolder(const std::filesystem::path& folderPath)
{
	if (!std::filesystem::is_directory(folderPath))
		return false;

	if (tileImages)
		resetTiles();

	const int numFiles = getNumFilesInFolder(folderPath);
	tileImages = new Image[numFiles];
	tileMeans = new Pixel[numFiles];

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(folderPath))
	{
		Image sourceImage;
		if (entry.is_regular_file() && sourceImage.load(entry.path().c_str()))
		{
			Image& tileImage = tileImages[numTileImages];
			sourceImage.cropToSquare(tileImage, tileSize, tileSize);

			Pixel& meanPixel = tileMeans[numTileImages];
			tileImage.computeTileMean(meanPixel, 0, 0, tileSize);

			numTileImages++;
		}
	}

	if (numTileImages == 0)
	{
		resetTiles();
		return false;
	}

	return true;
}

bool Mosaic::makeMosaicImage(Image& mosaicImage) const
{
	if (!isValid())
		return false;

	const int numTilesX = (int(sourceImage.getWidth() * scaling) + tileSize - 1) / tileSize;
	const int numTilesY = (int(sourceImage.getHeight() * scaling) + tileSize - 1) / tileSize;

	mosaicImage.init(numTilesX * tileSize, numTilesY * tileSize, sourceImage.getNumChannels());

	for (int tileY = 0; tileY < numTilesY; tileY++)
	{
		const int tileStartY = tileY * tileSize;

		for (int tileX = 0; tileX < numTilesX; tileX++)
		{
			const int tileStartX = tileX * tileSize;

			Pixel meanPixel;
			meanImage.readPixel(meanPixel, tileX, tileY);

			int closestMeanIndex = -1;
			float closestMeanDist = std::numeric_limits<float>::max();
			for (int tileIndex = 0; tileIndex < numTileImages; tileIndex++)
			{
				const Pixel& tileMean = tileMeans[tileIndex];

				const float meanDist = meanPixel.dist(tileMean);
				if (meanDist < closestMeanDist)
				{
					closestMeanIndex = tileIndex;
					closestMeanDist = meanDist;
				}
			}
			assert(closestMeanIndex >= 0);
			const Image& tile = tileImages[closestMeanIndex];

			mosaicImage.replaceTile(tile, tileStartX, tileStartY);
		}
	}

	return true;
}

int Mosaic::getNumFilesInFolder(const std::filesystem::path& folderPath)
{
	return std::count_if(
		std::filesystem::directory_iterator(folderPath),
		std::filesystem::directory_iterator{},
		[](const std::filesystem::directory_entry& entry)
		{
			return entry.is_regular_file();
		});
}
