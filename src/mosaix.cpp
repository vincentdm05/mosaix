
#include <Image.h>

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
	std::filesystem::path sourceFilename = argv[1];
	std::filesystem::path folderFilename = argv[2];

	std::cout << "Source: " << sourceFilename << std::endl;
	std::cout << "Folder: " << folderFilename << std::endl;

	Image sourceImage;
	sourceImage.load(sourceFilename.c_str());

	Image tileMeans;
	const int tileSize = 64;
	sourceImage.computeTileMeans(tileMeans, tileSize);

	Image croppedImage;
	const int newSize = tileSize;
	sourceImage.cropToSquare(croppedImage, newSize, newSize);

	std::filesystem::path destFilename(sourceFilename);
	destFilename.replace_extension(std::to_string(newSize) + ".png");
	croppedImage.write(destFilename.c_str());

	return 0;
}
