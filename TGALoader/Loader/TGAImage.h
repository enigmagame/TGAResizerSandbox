#pragma once

#include <string>

enum TGACODE
{
	TGA_ERR_OPEN,
	TGA_OK,
	TGA_ERR_BAD_FORMAT,
	TGA_ERR_UNSUPPORTED,
};

struct TGAHeader
{
	char idLength;
	char colorMapType;
	char imageType;

	short firstEntryIndex;
	short colorMapLength;
	char colorMapEntrySize;

	short xOrigin;
	short yOrigin;
	short width;
	short height;
	char pixelDepth;
	char imageDescriptor;
};

struct TGAData
{
	char* imageId;
	char* colorMapData;
	char* imageData;
};

struct TGAStruct
{
	TGAHeader header;
	TGAData data;
};

class TGAImage
{
public:
	TGAImage();
	TGAImage(const TGAImage& i_copy);
	~TGAImage();

	TGACODE Load(const std::string& i_fileName);
	TGACODE Save(const std::string& i_fileName);

	void Resize();

	size_t GetWidth() const;
	size_t GetHeight() const;
	size_t GetDepth() const;

private:
	TGAImage& operator=(const TGAImage&);

	TGACODE ReadHeader(std::ifstream& i_file);
	void WriteHeader(std::ofstream& i_file) const;

	void ReadData(std::ifstream& i_file);
	void WriteData(std::ofstream& i_file) const;

	TGAStruct m_struct;

	const static int TGA_HEADER_SIZE = 18;
};

inline size_t TGAImage::GetWidth() const
{
	return m_struct.header.width;
}

inline size_t TGAImage::GetHeight() const
{
	return m_struct.header.height;
}

inline size_t TGAImage::GetDepth() const
{
	return m_struct.header.pixelDepth;
}