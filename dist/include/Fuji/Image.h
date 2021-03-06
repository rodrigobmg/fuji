#if !defined(_IMAGE_H)
#define _IMAGE_H

typedef enum
{
	FUJI_RGBA,
	FUJI_BGRA,
	FUJI_RGB,
	FUJI_BGR
} ImageFormat;

class FujiImage
{
public:
	FujiImage();
	~FujiImage();
	void Convert(ImageFormat toFormat);
	void VFlip(void);

	uint32 width;
	uint32 height;
	uint8 bitsPerPixel;
	uint8 bytesPerPixel;

	ImageFormat format;

	void *pixels;
};

FujiImage* LoadTGA(const char *filename, bool flipped=true);

#endif
