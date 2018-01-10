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
};