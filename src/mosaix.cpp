
#include <Image.h>

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
	const char* sourceFilename = argv[1];
	const char* folderFilename = argv[2];

	std::cout << "Source: " << sourceFilename << std::endl;
	std::cout << "Folder: " << folderFilename << std::endl;

	Image destImage;
	destImage.load(sourceFilename);

	const int tileSize = 16;
	destImage.computeTileMeans(tileSize);

	std::string destFilename(sourceFilename);
	destFilename.append(".png");
	destImage.write(destFilename.c_str());

	return 0;
}
