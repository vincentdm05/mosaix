
#include <Image.h>

#include <iostream>

int main(int argc, char *argv[])
{
	const char* sourceFilename = argv[1];
	const char* folderFilename = argv[2];

	std::cout << "target: " << sourceFilename << std::endl;
	std::cout << "Folder: " << folderFilename << std::endl;

	Image targetImage;
	targetImage.load(sourceFilename);

	return 0;
}
