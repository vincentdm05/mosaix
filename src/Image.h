#pragma once

class Image
{
public:
	virtual ~Image();

	bool load(const char* filename);

private:
	int width = 0;
	int height = 0;
	unsigned char* data = nullptr;
};
