#include <Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>

Image::~Image()
{
	delete data;
}

bool Image::load(const char* filename)
{
	int channels;
	data = stbi_load(filename, &width, &height, &channels, 0);

	if (data == nullptr)
	{
		std::cerr << "Could not load image '" << filename << "'." << std::endl;
		return false;
	}

	std::cout << "Successfully loaded image '" << filename <<
		"' with size " << width << "x" << height <<
		" and " << channels << "channels." << std::endl;

	return true;
}
