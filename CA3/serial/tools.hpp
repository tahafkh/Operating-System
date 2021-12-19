#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <pthread.h>
#include <cmath>

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

extern int rows;
extern int cols;
extern int buffer_size;
extern char *file_buffer;

extern unsigned char*** photo_1;
extern unsigned char*** photo_2;

const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;


const int THREAD_PER_DIM = 2;


bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize);
void getPixlesFromBMP24(int end, int starting_row, int ending_row, int starting_col, int ending_col, char *fileReadBuffer);
void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize);

void allocate_photo(unsigned char***& photo);
void get_pixels_handler(int end, char *fileReadBuffer);


void filters_handler();

#endif