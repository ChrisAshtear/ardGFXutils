#include <stdio.h>
#include "rle.h"
#include <iostream>
#include <fstream>
#include "bmp.h"
#include "types.h"

#include <vector>

string usageMsg = "correct usage is rleCompress -switch inBMP outFile";
string switchMsg = "use -T for tileset, -M for tilemap, and -S for sprite";
string description = " is a utility to convert 8-bit BMP graphics to a usable (and compressed) format for use with Arduino.";
string outputFormat = " \nIt outputs an array of array of RLE_data.\nThis is a struct composed of 2 uint8s. 1st is the Pal index of the color to draw, 2nd is the amount of times that color repeats.\n";
string colorDepthError = "\nThis utiliy only support 8-bit BMP images. Image bit depth is: ";

int fileType = -1;
bool writeTileSet = false;
//need a better solution than the array, its bigger than original file many times.
//colorIndex should be array index
//[0]-{AddrStart/AddrEnd,AddrStart/AddrEnd



using namespace std;

void exportCode(char **argv, unsigned char *outblock,int outsize, int imgData[3])
{
	ofstream outfile (argv[4], ios::out|ios::trunc);
	if (outfile.is_open())
	{
		
		//outfile.write((char*)outblock,outsize);
		outfile << "uint8_t " << argv[2] << "height = " << imgData[0] << ";" <<endl;
		outfile << "uint8_t " << argv[2] << "width = " << imgData[1] << ";" <<endl;
		outfile << "uint8_t " << argv[2] << "numColors = " << imgData[2] << ";" <<endl;
		outfile << "PROGMEM " ;
		
		//---raw RLE
		outfile << argv[2] << "[" << outsize << "] = {";
		outfile << (uint8_t)outblock[outsize-1]+ 0;
		for(int i=outsize-2;i>=0;i--)
		{
			outfile <<",";
			outfile << (int) outblock[i] + 0<< "";
		}
		outfile << "};";
	
		outfile.close();
		delete[] outblock;
	}
}

string formatByte (uint8_t colorIdx, uint8_t rLength)
{
	int outByte;
	//cout << "IDX:" << colorIdx + 0 << " LEN: " << rLength + 0 << endl;
	if(rLength == 0)
	{
		outByte = colorIdx + 0;
		//loByte = "0";//add trailing 0 to hex number to make sure it outputs correctly
	}
	else
	{
		outByte = colorIdx * 16;
		//do this in RLE compression
		/*if(outblock[i].rLength > 16)
		{
			outByte += 16;
		}*/
		outByte += rLength;
	
	}
	char hex[8];
	itoa (outByte,hex,16);
	string output = "0x";
	if(colorIdx == 0)
	{
		output.append("0");
	}
	output.append(hex);
	if(rLength == 0)
	{
		output.append("0");
	}
	if(rLength == 0 && colorIdx == 0)
	{
		output = "0x00";
	}
	return output;
}

void exportCode(char **argv, RLE_data *outblock,int outsize, int imgData[3])
{
	ofstream outfile (argv[3], ios::out|ios::trunc);
	if (outfile.is_open())
	{
		
		//outfile.write((char*)outblock,outsize);
		outfile << "uint8_t " << argv[3] << "_height = " << imgData[0] << ";" <<endl;
		outfile << "uint8_t " << argv[3] << "_width = " << imgData[1] << ";" <<endl;
		outfile << "uint8_t " << argv[3] << "_numColors = " << imgData[2] << ";" <<endl;
		outfile << "PROGMEM " ;
		
		outfile << argv[3] << "[" << outsize << "] = {";

		outfile << formatByte(outblock[outsize-1].colorIdx,outblock[outsize-1].rLength);
		
		int height = imgData[0];
		int width = imgData[1];
		int tiles = height / width;
		
		int tileAddr[tiles];
		
		cout<< "tileLen:" << tiles;
		
		int pixCounter = 0;
		pixCounter +=outblock[outsize-1].rLength+1;
		int tCounter = tiles-1;
		for(int i=outsize-2;i>=0;i--)
		{
			outfile <<"," << formatByte(outblock[i].colorIdx,outblock[i].rLength);
			cout<< "," << formatByte(outblock[i].colorIdx,outblock[i].rLength);
			pixCounter +=outblock[i].rLength+1;
			if(pixCounter >= width*width && fileType == 0)
			{
				tileAddr[tCounter] = i;
				tCounter--;
				pixCounter -= width*width;
				cout<< "Addr:" << i;
			}
		}
		outfile << "};";
		if(fileType == 0)
		{
			outfile <<endl<< "ADDR[" << tiles << "] = {";
			outfile <<"0";//first tile addr will always be 0
			for(int i=1; i<tiles; i++)
			{
				outfile << ", " << tileAddr[i];
			}
			outfile << "};";
		}
		outfile.close();
		delete[] outblock;
	}
	string test;
}


int main(int argc, char **argv)
{
	streampos size;
	unsigned char * memblock;
	unsigned char * outblock;
	int outsize = 0;
	//argv 0-executable name, argv1 first arg, etc.
	/*argc = 3;
	argv[1] = "-T";
	argv[2] = "fsheet2.bmp";
	argv[3] = "ts";
	argv[4] = "tfrle";*/
	//support read-in of tilemap files.
	char typeSwitch = argv[1][1];
	cout << typeSwitch;
	switch(typeSwitch)
	{
		case 'T'://TileSet
		fileType = 0;
		writeTileSet = true;
		break;
		
		case 'S'://Sprite
		fileType = 1;
		break;
		
		case 'M'://TileMap
		fileType = 2;
		break;
	};
	
	if(argc < 3 || fileType == -1)
	{
		cout << endl << argv[0] << description <<endl <<endl;
		cout << usageMsg <<endl << switchMsg << endl << outputFormat;
		return 0;
	}

	
	
	BMPobj bitmap = readBMP(argv[2]);
	
	if(bitmap.bitDepth != 8)
	{
		cout << colorDepthError << bitmap.bitDepth <<endl;
		return 0;
	}
	
	size = bitmap.bmp.size();
	cout << "\nthe entire file content is in memory\n";
	cout << "size = " << size << "\n";
	
	int outBufferSize = (size * 2) + 1;
	
	outblock = new unsigned char [outBufferSize];	
	outsize = RLE_Compress( bitmap.bmp.data(), outblock, size);
	
	cout << "\n\nfinished compression\n";
	cout << "new size = " << outsize << "\n";
	
	//delete[] memblock;
	
	cout << "\nbeginning conversion to array\n";
	
	
	int imgData[3] = {bitmap.imgHeight , bitmap.imgWidth , bitmap.numColors};
	
	
	//GENERATE RLE SHAPES to draw. need some kind of routine to find least amount of rects per
	//image, with different tolerances for including whitespace
	
	//RLE_data* outRLE = new RLE_data [outBufferSize];	
	unsigned char* out = new unsigned char[outBufferSize];
	//int arrSize = RLE_Uncompress( outblock, out, outsize);
	
	
	//exportCode(argv,outblock,outsize,imgData);
	
	RLE_data* outRLE = new RLE_data [outBufferSize];	
	
	
	int arrSize = RLE_Uncompress( outblock, outRLE, outsize,writeTileSet);
	
	//exportCode(argv,outblock,outsize,imgData);
	exportCode(argv,outRLE,arrSize,imgData);
	
	delete[] outblock;
	
  return 0;
}
