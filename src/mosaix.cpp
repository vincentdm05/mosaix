
#include <Image.h>

#include <iostream>

int main(int argc, char *argv[])
{
	const char* targetFilename = argv[1];
	const char* folderFilename = argv[2];

	std::cout << "target: " << targetFilename << std::endl;
	std::cout << "Folder: " << folderFilename << std::endl;

	Image targetImage;
	targetImage.load(targetFilename);

	return 0;
}
