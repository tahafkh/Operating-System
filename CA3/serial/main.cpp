#include "tools.hpp"

int rows;
int cols;
int buffer_size;
char *file_buffer;

unsigned char*** photo_1;
unsigned char*** photo_2;


using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

int main(int argc, char *argv[])
{
  char *file_name = argv[1];

  if (!fillAndAllocate(file_buffer, file_name, rows, cols, buffer_size))
  {
    cout << "Error reading file" << endl;
    return 1;
  }

  allocate_photo(photo_1);
  allocate_photo(photo_2);


  return 0;
}