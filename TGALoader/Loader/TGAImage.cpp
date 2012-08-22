#include "TGAImage.h"
#include <fstream>
#include <iostream>

using namespace std;

TGAImage::TGAImage()
	:m_struct(TGAStruct())
{
}

TGAImage::TGAImage(const TGAImage& i_copy)
{
	m_struct.header = i_copy.m_struct.header;

	const size_t imageIdSize = i_copy.m_struct.header.idLength;
	m_struct.data.imageId = new char[imageIdSize];

	for (size_t i = 0; i < imageIdSize; ++i)
		m_struct.data.imageId[i] = i_copy.m_struct.data.imageId[i];

	const size_t colorMapDataSize = i_copy.m_struct.header.colorMapLength;
	m_struct.data.colorMapData = new char[colorMapDataSize];

	for (size_t i = 0; i < colorMapDataSize; ++i)
		m_struct.data.colorMapData[i] = i_copy.m_struct.data.colorMapData[i];

	const size_t imageDataSize = (i_copy.GetWidth() * i_copy.GetHeight() * (i_copy.GetDepth() / 8));
	m_struct.data.imageData = new char[imageDataSize];

	for (size_t i = 0; i < imageDataSize; ++i)
		m_struct.data.imageData[i] = i_copy.m_struct.data.imageData[i];
}

TGAImage::~TGAImage()
{
	delete[] m_struct.data.imageId;	
	delete[] m_struct.data.colorMapData;
	delete[] m_struct.data.imageData;
}

TGACODE TGAImage::Load(const string& i_fileName)
{
	ifstream file;
	file.open(i_fileName, ios::in | ios::binary);

	if (file.is_open())
	{
		TGACODE result = ReadHeader(file);

		if (result == TGA_OK)
		{
			ReadData(file);
			file.close();
			return result;
		}
		else
		{
			return result;
		}
	}
	else
	{
		return TGA_ERR_OPEN;
	}
}

TGACODE TGAImage::Save(const string& i_fileName)
{
	ofstream file;
	file.open(i_fileName, ios::out | ios::binary);

	if (file.is_open())
	{
		WriteHeader(file);
		WriteData(file);
		file.close();
		return TGA_OK;
	}
	else
	{
		return TGA_ERR_OPEN;
	}
}

void TGAImage::Resize()
{
	m_struct.header.width /= 2;
	m_struct.header.height /= 2;

	const size_t imageSize = (m_struct.header.width * m_struct.header.height);
	const size_t pixelDepth = (m_struct.header.pixelDepth / 8);
	const size_t size = imageSize * pixelDepth;

	char* imageData = new char[size];

	int rowOffset = 0;

	for (size_t i = 0; i < imageSize; ++i)
	{
		for (size_t j = 0; j < pixelDepth; ++j)
		{
			imageData[((i * pixelDepth) + j)] = m_struct.data.imageData[((((i * pixelDepth) * 2) + j) + rowOffset)];
		}

		if (i && ((i % m_struct.header.width) == 0))
		{
			rowOffset += (m_struct.header.width * 2 * pixelDepth);
		}
	}

	if (m_struct.data.imageData != 0)
		delete[] m_struct.data.imageData;

	m_struct.data.imageData = new char[size];

	for (size_t i = 0; i < size; ++i)
	{
		m_struct.data.imageData[i] = imageData[i];
	}

	delete[] imageData;
}

TGACODE TGAImage::ReadHeader(ifstream& i_file)
{
	char tmpHeader[TGA_HEADER_SIZE];

	i_file.read(tmpHeader, sizeof(tmpHeader));

	m_struct.header.idLength = tmpHeader[0];
	m_struct.header.colorMapType = tmpHeader[1];
	m_struct.header.imageType = tmpHeader[2];

	m_struct.header.firstEntryIndex = (tmpHeader[3] & 0xFF) | (tmpHeader[4] << 8);
	m_struct.header.colorMapLength = (tmpHeader[5] & 0xFF) | (tmpHeader[6] << 8);
	m_struct.header.colorMapEntrySize = tmpHeader[7];

	m_struct.header.xOrigin = (tmpHeader[8] & 0xFF) | (tmpHeader[9] << 8);
	m_struct.header.yOrigin = (tmpHeader[10] & 0xFF) | (tmpHeader[11] << 8);
	m_struct.header.width = (tmpHeader[12] & 0xFF) | (tmpHeader[13] << 8);
	m_struct.header.height = (tmpHeader[14] & 0xFF) | (tmpHeader[15] << 8);
	m_struct.header.pixelDepth = tmpHeader[16];
	m_struct.header.imageDescriptor = tmpHeader[17];

	if (m_struct.header.colorMapType != 0)
		return TGA_ERR_UNSUPPORTED;
	if (m_struct.header.imageType != 2)
		return TGA_ERR_UNSUPPORTED;
	if ((m_struct.header.width < 1) || (m_struct.header.height < 1))
		return TGA_ERR_BAD_FORMAT;
	if (m_struct.header.pixelDepth > 32)
		return TGA_ERR_UNSUPPORTED;

	return TGA_OK;
}

void TGAImage::WriteHeader(ofstream& i_file) const
{
	char tmpHeader[TGA_HEADER_SIZE];

	tmpHeader[0] = m_struct.header.idLength;
	tmpHeader[1] = m_struct.header.colorMapType;
	tmpHeader[2] = m_struct.header.imageType;

	tmpHeader[3] = static_cast<char>(m_struct.header.firstEntryIndex);
	tmpHeader[4] = (m_struct.header.firstEntryIndex >> 8);

	tmpHeader[5] = static_cast<char>(m_struct.header.colorMapLength);
	tmpHeader[6] = (m_struct.header.colorMapLength >> 8);

	tmpHeader[7] = m_struct.header.colorMapEntrySize;

	tmpHeader[8] = static_cast<char>(m_struct.header.xOrigin);
	tmpHeader[9] = (m_struct.header.xOrigin >> 8);

	tmpHeader[10] = static_cast<char>(m_struct.header.yOrigin);
	tmpHeader[11] = (m_struct.header.yOrigin >> 8);

	tmpHeader[12] = static_cast<char>(m_struct.header.width);
	tmpHeader[13] = (m_struct.header.width >> 8);

	tmpHeader[14] = static_cast<char>(m_struct.header.height);
	tmpHeader[15] = (m_struct.header.height >> 8);

	tmpHeader[16] = m_struct.header.pixelDepth;
	tmpHeader[17] = m_struct.header.imageDescriptor;

	i_file.write(tmpHeader, TGA_HEADER_SIZE);
}

void TGAImage::ReadData(ifstream& i_file)
{
	const size_t imageDataSize = (m_struct.header.width * m_struct.header.height * (m_struct.header.pixelDepth / 8));

	m_struct.data.imageData = new char[imageDataSize];

	i_file.read(m_struct.data.imageData, imageDataSize);
}

void TGAImage::WriteData(ofstream& i_file) const
{
	int imageDataSize = (m_struct.header.width * m_struct.header.height * (m_struct.header.pixelDepth / 8));

	i_file.write(m_struct.data.imageData, imageDataSize);
}
