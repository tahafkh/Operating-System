#include "tools.hpp"


using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

void allocate_photo(unsigned char***& photo)
{
    photo = new unsigned char**[rows];
    for (int i = 0; i < rows; i++){
        photo[i] = new unsigned char*[cols];
        for(int j = 0; j < cols; j++){
            photo[i][j] = new unsigned char[3];
        }
    }
}

int find_ending(int starting)
{
    return starting + rows/THREAD_PER_DIM;
}

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int starting_row, int ending_row, int starting_col, int ending_col, char *fileReadBuffer)
{
  int count = 1;
  int extra = cols % 4 + starting_row * cols * 3;
  for (int i = starting_row; i < ending_row; i++)
  {
    count += extra + (cols - ending_col)*3;
    for (int j = ending_col - 1; j >= starting_col; j--){
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          photo_1[i][j][RED] = fileReadBuffer[end - count];
          break;
        case 1:
          photo_1[i][j][GREEN] = fileReadBuffer[end - count];
          break;
        case 2:
          photo_1[i][j][BLUE] = fileReadBuffer[end - count];
          break;
        }
        count++;
      }
    }
    count += starting_col * 3;
  }
}


void get_pixels_handler(int end, char *fileReadBuffer)
{
    getPixlesFromBMP24(end, 0, rows, 0, rows, fileReadBuffer);
}

void write_to_buffer(int end, int starting_row, int ending_row, int starting_col, int ending_col, char *fileWriteBuffer)
{
  int count = 1;
  int extra = cols % 4 + starting_row * cols * 3;
  for (int i = starting_row; i < ending_row; i++)
  {
    count += extra + (cols - ending_col)*3;
    for (int j = ending_col - 1; j >= starting_col; j--){
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileWriteBuffer[end - count] = photo_1[i][j][RED];
          break;
        case 1:
          fileWriteBuffer[end - count] = photo_1[i][j][GREEN];
          break;
        case 2:
          fileWriteBuffer[end - count] = photo_1[i][j][BLUE];
          break;
        }
        count++;
      }
    }
    count += starting_col * 3;
  }
}

void write_handler(char* fileBuffer)
{
    write_to_buffer(buffer_size, 0, rows, 0, cols, fileBuffer);
}


void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  write_handler(fileBuffer);
  write.write(fileBuffer, bufferSize);
}
