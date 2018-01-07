#include <stdio.h>
#include "rle.h"
#include <iostream>
#include <fstream>
#include "bmp.h"
#include "types.h"

#include <vector>

string usageMsg = "correct usage is rleCompress inBMP outFile";
string description = " is a utility to convert 8-bit BMP graphics to a usable (and compressed) format for use with Arduino.";
string outputFormat = " \nIt outputs an array of array of RLE_data.\nThis is a struct composed of 2 uint8s. 1st is the Pal index of the color to draw, 2nd is the amount of times that color repeats.\n";
string colorDepthError = "\nThis utiliy only support 8-bit BMP images. Image bit depth is: ";


using namespace std;

void exportCode(char **argv, RLE_data *outblock,int outsize, int imgData[3])
{
	ofstream outfile (argv[2], ios::out|ios::trunc);
	if (outfile.is_open())
	{
		
		//outfile.write((char*)outblock,outsize);
		outfile << "uint8_t " << argv[2] << "height = " << imgData[0] << ";" <<endl;
		outfile << "uint8_t " << argv[2] << "width = " << imgData[1] << ";" <<endl;
		outfile << "uint8_t " << argv[2] << "numColors = " << imgData[2] << ";" <<endl;
		outfile << "PROGMEM " ;
		outfile << argv[2] << "[" << outsize << "] = {{";
		outfile << (uint8_t)outblock[0].colorIdx + 0<< ", " << outblock[0].rLength << "}";
		for(int i=1;i< outsize;i++)
		{
			outfile <<",{";
			outfile << (uint8_t)outblock[i].colorIdx + 0<< ", " << outblock[i].rLength << "}";
		}
		outfile << "};";
		outfile.close();
		delete[] outblock;
	}
}

int main(int argc, char **argv)
{
	streampos size;
	unsigned char * memblock;
	unsigned char * outblock;
	int outsize = 0;
	//argv 0-executable name, argv1 first arg, etc.
	
	if(argc < 2)
	{
		cout << endl << argv[0] << description <<endl <<endl;
		cout << usageMsg <<endl << outputFormat;
		return 0;
	}
	
	BMPobj bitmap = readBMP(argv[1]);
	
	if(bitmap.bitDepth != 8)
	{
		cout << colorDepthError << bitmap.bitDepth <<endl;
		return 0;
	}
	
	size = bitmap.bmp.size();
	cout << "\nthe entire file content is in memory\n";
	cout << "size = " << size << "\n";
	
	int outBufferSize = (size * 1.04) + 1;
	
	outblock = new unsigned char [outBufferSize];	
	outsize = RLE_Compress( bitmap.bmp.data(), outblock, size);
	
	cout << "\n\nfinished compression\n";
	cout << "new size = " << outsize << "\n";
	
	//delete[] memblock;
	
	cout << "\nbeginning conversion to array\n";
	
	
	int imgData[3] = {bitmap.imgHeight , bitmap.imgWidth , bitmap.numColors};
	
	
	//GENERATE RLE SHAPES to draw. need some kind of routine to find least amount of rects per
	//image, with different tolerances for including whitespace
	
	RLE_data* outRLE = new RLE_data [outBufferSize];	
	int arrSize = RLE_Uncompress( outblock, outRLE, outsize);
	
	
	exportCode(argv,outRLE,arrSize,imgData);
	
	delete[] outblock;
	delete[] outRLE;
	
  return 0;
}
