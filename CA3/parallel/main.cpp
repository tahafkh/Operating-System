#include "tools.hpp"
#include <chrono>

int rows;
int cols;
int buffer_size;
char *file_buffer;
float color_sums[TOTAL_THREADS][3] = {0};
float final_sums[3] = {0};

unsigned char*** photo_1;
unsigned char*** photo_2;

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std::chrono;

int main(int argc, char *argv[])
{
  char *file_name = argv[1];
  const char* OUTPUT_PHOTO_NAME = "output.bmp";
  
  auto start = high_resolution_clock::now();
  if (!fillAndAllocate(file_buffer, file_name, rows, cols, buffer_size))
  {
    cout << "Error reading file" << endl;
    return 1;
  }

  allocate_photo(photo_1);
  allocate_photo(photo_2);

  get_pixels_handler(buffer_size, file_buffer);

  filters_handler();

  writeOutBmp24(file_buffer, OUTPUT_PHOTO_NAME, buffer_size);
  auto stop = high_resolution_clock::now();

  auto duration = duration_cast<milliseconds>(stop - start);

  cout << "Duration: " << duration.count() << "ms" << endl;
  return 0;
}