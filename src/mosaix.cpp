
#include <Image.h>

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
	const std::filesystem::path imagePath = argv[1];
	const std::filesystem::path folderPath = argv[2];
	const std::filesystem::path tilesFolderPath = folderPath / "tiles";

	std::cout << "Source image: " << imagePath << std::endl;
	std::cout << "Source folder: " << folderPath << std::endl;
	std::cout << "Output tiles folder: " << tilesFolderPath << std::endl;

	if (!std::filesystem::is_directory(tilesFolderPath) && !std::filesystem::create_directory(tilesFolderPath))
	{
		std::cerr << "Could not create directory '" << tilesFolderPath << "'.";
		return 1;
	}

	const int tileSize = 64;

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(folderPath))
	{
		Image sourceImage;
		if (entry.is_regular_file() && sourceImage.load(entry.path().c_str()))
		{
			Image tileImage;
			sourceImage.cropToSquare(tileImage, tileSize, tileSize);

			std::filesystem::path tileFilename = tilesFolderPath / entry.path().filename();
			tileFilename.replace_extension(std::to_string(tileSize) + ".png");
			tileImage.write(tileFilename.c_str());
		}
	}

	return 0;
}
