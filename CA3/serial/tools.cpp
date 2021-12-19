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
          fileWriteBuffer[end - count] = photo_2[i][j][RED];
          break;
        case 1:
          fileWriteBuffer[end - count] = photo_2[i][j][GREEN];
          break;
        case 2:
          fileWriteBuffer[end - count] = photo_2[i][j][BLUE];
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

bool is_valid(int i, int j){
    return i >= 0 && i < rows && j >= 0 && j < cols;
}

void add_smoothing_filter(int starting_row, int ending_row, int starting_col, int ending_col){
    for(int i = starting_row; i < ending_row; i++){
        for(int j = starting_col; j < ending_col; j++){
            int sums[3] = {0, 0, 0};
            for(int delta_i = -1; delta_i < 2; delta_i++){
                for(int delta_j = -1; delta_j < 2; delta_j++){
                    if(is_valid(i + delta_i, j + delta_j)){
                        for(int color = 0; color < 3; color++){
                            sums[color] += int(photo_1[i+delta_i][j+delta_j][color]);
                        }
                    }
                }
            }
            for(int color = 0; color < 3; color++){
                photo_2[i][j][color] = (unsigned char)(sums[color]/9);
            }
        }
    }
}

void add_sepia_filter(int starting_row, int ending_row, int starting_col, int ending_col, float* color_sums){
    const float coefs[3][3] = {{0.393, 0.769, 0.189}, {0.349, 0.686, 0.168}, {0.272, 0.534, 0.131}};
    for(int i = starting_row; i < ending_row; i++){
        for(int j = starting_col; j < ending_col; j++){
            for(int main_color = 0; main_color < 3; main_color++){
                float new_value = 0;
                for(int color = 0; color < 3; color++){
                    new_value += coefs[main_color][color] * photo_2[i][j][color];
                }
                photo_1[i][j][main_color] = (unsigned char)(fmin(round(new_value), 255));
                color_sums[main_color] += photo_1[i][j][main_color];
            }
        }
    }
}

void add_mean_filter(int starting_row, int ending_row, int starting_col, int ending_col, float* color_sums)
{
    for(int i = starting_row; i < ending_row; i++){
        for(int j = starting_col; j < ending_col; j++){
            for(int color = 0; color < 3; color++){
                float color_mean = color_sums[color] / (rows * cols);
                photo_2[i][j][color] = (unsigned char)(fmin(round(0.4*photo_1[i][j][color] + 0.6*color_mean), 255));
            }
        }
    }
}

void draw_cross(int i, int j){
    for(int delta = -1; delta < 2; delta++){
        if(is_valid(i, j+delta)){
            for(int color = 0; color < 3; color++){
                photo_2[i][j+delta][color] = 255;
            }
        }
    }
}


void add_cross_filter(int starting_row, int ending_row, int starting_col, int ending_col, bool draw_first_cross, bool draw_second_cross)
{
    for(int i = starting_row; i < ending_row; i++){
        int j = i - starting_row;
        if(draw_first_cross){
            draw_cross(i, j + starting_col);
        }

        if(draw_second_cross){
            draw_cross(i, ending_col - j);    
        }
    }
}

void smoothing_filter_handler(){
    add_smoothing_filter(0, rows, 0, cols);
}

void sepia_filter_handler(float* color_sums){
    add_sepia_filter(0, rows, 0, cols, color_sums);
}

void mean_filter_handler(float* color_sums){
    add_mean_filter(0, rows, 0, cols, color_sums);
}

void cross_filter_handler(){
    add_cross_filter(0, rows, 0, cols, true, true);
}

void filters_handler(){
    float color_sums[3] = {0, 0, 0};
    smoothing_filter_handler();
    sepia_filter_handler(color_sums);
    mean_filter_handler(color_sums);
    cross_filter_handler();
}