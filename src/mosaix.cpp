
#include <Image.h>
#include <Mosaic.h>

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

struct App
{
	virtual void setArgs(int argc, char *argv[]) = 0;
	virtual void run() = 0;
};

class AppPassthrough : public App
{
private:
	std::filesystem::path imagePath;

public:
	void setArgs(int argc, char *argv[]) override
	{
		assert(argc > 1);
		imagePath = argv[1];
	}
	void run() override
	{
		Image image;
		image.load(imagePath.c_str());
		std::filesystem::path outputPath(imagePath);
		outputPath.replace_extension("passthrough.png");
		image.write(outputPath.c_str());
	}
};

class AppCropResizeSingleImage : public App
{
private:
	std::filesystem::path imagePath;

public:
	void setArgs(int argc, char *argv[]) override
	{
		assert(argc > 1);
		imagePath = argv[1];
	}
	void run() override
	{
		Image image;
		image.load(imagePath.c_str());
		Image croppedImage;
		image.cropToSquare(croppedImage);
		std::filesystem::path outputPath(imagePath);
		outputPath.replace_extension("cropped.png");
		croppedImage.write(outputPath.c_str());
	}
};

class AppMosaic : public App
{
private:
	std::filesystem::path imagePath;
	std::filesystem::path folderPath;
	float scaling = 1.0f;
	int tileSize = -1.0f;

public:
	void setArgs(int argc, char *argv[]) override
	{
		assert(argc > 2);
		imagePath = argv[1];
		folderPath = argv[2];
		if (argc > 3)
		{
			scaling = std::stof(argv[3]);
			scaling = scaling <= 0.01f ? 0.01f : scaling;
			scaling = scaling > 10.0f ? 10.0f : scaling;
		}
		if (argc > 4)
		{
			tileSize = std::stoi(argv[4]);
			tileSize = tileSize < 1 ? 1 : tileSize;
			tileSize = tileSize > 4096 ? 4096 : tileSize;
		}
		else
		{
			tileSize = 16;
		}
	}
	void run() override
	{
		Mosaic mosaic;
		mosaic.setTileSize(tileSize);
		mosaic.setScaling(scaling);
		mosaic.setSourceImage(imagePath);
		mosaic.setTilesFolder(folderPath);
		Image mosaicImage;
		mosaic.makeMosaicImage(mosaicImage);

		std::filesystem::path outputPath(imagePath);
		outputPath.replace_extension("mosaic_x" + std::to_string(scaling) + "_" + std::to_string(tileSize) + ".png");
		mosaicImage.write(outputPath.c_str());
	}
};

int main(int argc, char *argv[])
{
	// AppPassthrough app;
	// AppCropResizeSingleImage app;
	AppMosaic app;
	app.setArgs(argc, argv);
	app.run();

	return 0;
}
