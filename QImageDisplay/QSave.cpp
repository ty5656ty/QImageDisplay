#include "QSave.h"

#include <tiff.h>
#include <tiffio.h>
#pragma comment(lib,"libtiff.lib")
#pragma comment(lib,"libtiff_i.lib")

QSave::QSave()
{
}


QSave::~QSave()
{
}

void QSave::start(void *file, int width, int height, int bytestep, Priority pri)
{
	bufferwidth = width;
	bufferheight = height;
	bufferlen = width * height * bytestep;
	tiff = file;
	QThreadProc::start(pri);
}

void QSave::run()
{
	buffer = new char[bufferlen];
	TIFF *writer = (TIFF*)tiff;
	if (writer == NULL)
		return;
	unsigned char *data = (unsigned char*)buffer;
	
	while (bRunning)
	{
		waitData();
		TIFFSetField(writer, TIFFTAG_IMAGEWIDTH, bufferwidth);
		TIFFSetField(writer, TIFFTAG_IMAGELENGTH, bufferheight);
		TIFFSetField(writer, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(writer, TIFFTAG_SAMPLESPERPIXEL, 3);
		TIFFSetField(writer, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(writer, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(writer, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
		TIFFSetField(writer, TIFFTAG_ROWSPERSTRIP, bufferheight);
		TIFFSetField(writer, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
		TIFFWriteEncodedStrip(writer, 0, data, bufferlen);
		TIFFWriteDirectory(writer);
	}

	delete[] (unsigned char*)buffer;
}

void QSave::DataSlot(void *dataptr)
{
	unsigned char* ptr1 = (unsigned char*)buffer;
	unsigned char* ptr2 = (unsigned char*)dataptr;
	for (int i = 0; i < bufferheight; i++)
	{
		for (int j = 0; j < bufferwidth; j++)
		{
			ptr1[j * 3 + 0 + i * bufferwidth * 3] = ptr2[j * 4 + 2 + i * bufferwidth * 4];
			ptr1[j * 3 + 1 + i * bufferwidth * 3] = ptr2[j * 4 + 1 + i * bufferwidth * 4];
			ptr1[j * 3 + 2 + i * bufferwidth * 3] = ptr2[j * 4 + 0 + i * bufferwidth * 4];
		}
	}
	setData();
}