#ifndef _types_h_
#define _types_h_

#include <vector>
using namespace std;

typedef struct RLE_data
{
  char colorIdx;
  int rLength;
};

typedef struct BMPobj
{
	vector<unsigned char> bmp;
	int numColors;
	int imgWidth;
	int imgHeight;
	int bitDepth; 
	char* pal;
};

#endif