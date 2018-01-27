#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>
#include "types.h"

using namespace std;

BMPobj readBMP(const string &file)
{
    static constexpr size_t HEADER_SIZE = 54;
	
	BMPobj returnObj;
	
	//palette starts immediately after header.

    ifstream bmp(file, ios::binary);

    array<char, HEADER_SIZE> header;
    bmp.read(header.data(), header.size());

    auto fileSize = *reinterpret_cast<uint32_t *>(&header[2]);
    auto dataOffset = *reinterpret_cast<uint32_t *>(&header[10]);
    auto width = *reinterpret_cast<uint32_t *>(&header[18]);
    auto height = *reinterpret_cast<uint32_t *>(&header[22]);
    auto depth = *reinterpret_cast<uint16_t *>(&header[28]);
	auto numColors = *reinterpret_cast<uint32_t *>(&header[46]);
	
	returnObj.imgWidth = width;
	returnObj.imgHeight = height;
	returnObj.numColors = numColors;
	returnObj.bitDepth = depth;
	
	cout << endl << "_Header Info_" <<endl;
    cout << "fileSize: " << fileSize << endl;
    cout << "dataOffset: " << dataOffset << endl;
    cout << "width: " << width << endl;
    cout << "height: " << height << endl;
    cout << "depth: " << depth << "-bit" << endl ;
	cout << "numColors: " << (int)numColors << endl;
	//cout << "header: " ;
	
	
	for(int i=30; i< HEADER_SIZE; i+=4)
	{
		//cout << i << " - " << *reinterpret_cast<uint32_t*>(&header[i]) << endl;
	}
	
	/*for(int i=HEADER_SIZE;i<((numColors*4) + HEADER_SIZE);i+=4)
	{
		//grab palette colors.
	}*/

    vector<unsigned char> img(width*height);
	
	int imgLength = width*height;
	
	char * pal = new char[numColors*4];
	bmp.read(pal,numColors*4);

	for(int i =0; i<numColors*4; i++)
	{
		cout << (uint8_t)pal[i]+0 << endl;
	}
    bmp.read((char*)img.data(), imgLength);
	
	for(int i =0; i<imgLength; i++)
	{
		//cout << (int)img[i] << endl;
	}
	
	/*for(int i = 0; i < fileSize; i++)
	{
		
	}*/
	returnObj.bmp = img;

    return returnObj;
}