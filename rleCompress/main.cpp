#include <stdio.h>
#include "rle.h"
#include <iostream>
#include <fstream>
#include "bmp.h"
#include "types.h"
#include <sstream>
#include <vector>
#include "tmxparser.h"

#define DEPTH_SCALE 5


#define printf_depth( depth, format, ... ) \
	printf("%*s " format "\n", ((depth)*DEPTH_SCALE), "", __VA_ARGS__)


string usageMsg = "correct usage is rleCompress -switch inBMP outFile";
string switchMsg = "use -T for tileset, -M for tilemap, and -S for sprite";
string description = " is a utility to convert 8-bit BMP graphics to a usable (and compressed) format for use with Arduino.";
string outputFormat = " \nIt outputs an array of array of RLE_data.\nThis is a struct composed of 2 uint8s. 1st is the Pal index of the color to draw, 2nd is the amount of times that color repeats.\n";
string colorDepthError = "\nThis utiliy only support 8-bit BMP images. Image bit depth is: ";

int fileType = -1;
bool writeTileSet = false;

uint16_t convertToRGB565(int pal[3]);
//need a better solution than the array, its bigger than original file many times.
//colorIndex should be array index
//[0]-{AddrStart/AddrEnd,AddrStart/AddrEnd



using namespace std;

void printTmxMapData(const tmxparser::TmxMap* map)
{
	int depth = 0;
	
	printf_depth(0, "%s", "<map>");
	depth = 1;
	printf_depth(depth, "Version: %s", map->version.c_str());
	printf_depth(depth, "Orientation: %d", map->orientation);
	printf_depth(depth, "Width: %u", map->width);
	printf_depth(depth, "Height: %u", map->height);
	printf_depth(depth, "TileWidth: %u", map->tileWidth);
	printf_depth(depth, "TileHeight: %u", map->tileHeight);
	printf_depth(depth, "BackgroundColor: %s", map->backgroundColor.c_str());
	/*printProperties(depth+1, map->propertyMap);
	printTilesets(depth+1, map->tilesetCollection);
	printLayers(depth+1, map->layerCollection);
	printObjectGroups(depth+1, map->objectGroupCollection);*/
}

tmxparser::TmxMap readTMX(string filename)
{
	printf("tmxparser::main()\n");

	tmxparser::TmxReturn error;
	tmxparser::TmxMap map;

	// test from file
	error = tmxparser::parseFromFile(filename, &map, "");
	cout << "test";
	if (!error)
	{
		printTmxMapData(&map);

		tmxparser::TmxRect rect;
		rect.u = 0; rect.v = 0; rect.u2 = 0; rect.v2 = 0;
		for (auto it : map.layerCollection[0].tiles)
		{
			tmxparser::calculateTileCoordinatesUV(map.tilesetCollection[it.tilesetIndex], it.tileFlatIndex, 0.5f, true, rect);
			printf("Tileset[%u]@Tile[%u]=Rect( (%f, %f)->(%f, %f) )\n", it.tilesetIndex, it.tileFlatIndex, rect.u, rect.v, rect.u2, rect.v2);
		}
		
	}
	else
	{
		printf("error parsing file");
	}

	return map;
}

string writeBMPdata(string name, BMPobj bmp)
{
	//write BMP header info to output file
	stringstream out;
	string output;
	
	stringstream pal16;
	
	out << "uint8_t " << name << "_height = " << bmp.imgHeight << ";" <<endl;
	out << "uint8_t " << name << "_width = " << bmp.imgWidth << ";" <<endl;
	out << "uint8_t " << name << "_numColors = " << bmp.numColors << ";" <<endl;
	
	uint8_t color_hi;
	uint8_t color_lo;

	pal16 << "uint16_t " << name << "_pal[" << bmp.numColors << "] = {";
	
	for(int i = 0; i<bmp.numColors*4;i+=4)
	{
		int rgb888[3] = {bmp.pal[i+2],bmp.pal[i+1],bmp.pal[i]};
		uint16_t color = convertToRGB565(rgb888);
		
		color_hi = color >> 8;
		color_lo = color;
		
		pal16 << "," <<hex<< color;
	}
	
	pal16 << "};";
	//out << endl << pal_l.str() << endl << pal_h.str() << endl;
	out << endl << pal16.str() << endl;
	output = out.str();
	return output;
}

uint16_t convertToRGB565(int pal[3])
{
    uint8_t red   = pal[0];
    uint8_t green = pal[1];
    uint8_t blue  = pal[2];

    uint16_t b = (blue >> 3) & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3) & 0x1f) << 11;

    return (uint16_t) (r | g | b);
}

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

void exportCode(char **argv, RLE_data *outblock,int outsize, BMPobj bmp)
{
	ofstream outfile (argv[3], ios::out|ios::trunc);
	if (outfile.is_open())
	{
		
		//outfile.write((char*)outblock,outsize);
		string header = writeBMPdata(argv[3],bmp);
		outfile << header;
		outfile << "PROGMEM " ;
		
		outfile << argv[3] << "[" << outsize << "] = {";

		outfile << formatByte(outblock[outsize-1].colorIdx,outblock[outsize-1].rLength);
		
		int height = bmp.imgHeight;
		int width = bmp.imgWidth;
		int tiles = height / width;
		
		int tileAddr[tiles];
		
		cout<< "tileLen:" << tiles;
		
		int pixCounter = 0;
		pixCounter +=outblock[outsize-1].rLength+1;
		int tCounter = 0;
		for(int i=outsize-2;i>=0;i--)
		{
			outfile <<"," << formatByte(outblock[i].colorIdx,outblock[i].rLength);
			cout<< "," << formatByte(outblock[i].colorIdx,outblock[i].rLength);
			pixCounter +=outblock[i].rLength+1;
			if(pixCounter >= width*width && fileType == 0)
			{
				tileAddr[tCounter] = outsize - i;
				tCounter++;
				pixCounter -= width*width;
				cout<< "Addr:" << i;
			}
		}
		outfile << "};";
		if(fileType == 0)
		{
			outfile <<endl<< "ADDR[" << tiles << "] = {";
			outfile <<"0";//first tile addr will always be 0
			for(int i=0; i<tiles-1; i++)
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
	argc = 3;
	argv[1] = "-M";
	argv[2] = "element0.tmx";
	argv[3] = "ts";
	argv[4] = "tfrle";
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
	
	BMPobj bitmap;
	tmxparser::TmxMap tileMap;
	
	if(fileType !=2)
	{
	
		bitmap = readBMP(argv[2]);
		
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
		
		//GENERATE RLE SHAPES to draw. need some kind of routine to find least amount of rects per
		//image, with different tolerances for including whitespace
		
		//RLE_data* outRLE = new RLE_data [outBufferSize];	
		unsigned char* out = new unsigned char[outBufferSize];
		//int arrSize = RLE_Uncompress( outblock, out, outsize);
		
		
		//exportCode(argv,outblock,outsize,imgData);
		
		RLE_data* outRLE = new RLE_data [outBufferSize];	
		
		
		int arrSize = RLE_Uncompress( outblock, outRLE, outsize,writeTileSet);
		
		//exportCode(argv,outblock,outsize,imgData);
		exportCode(argv,outRLE,arrSize,bitmap);
		
		delete[] outblock;
	}
	else
	{
		tileMap = readTMX((string)argv[2]);
	}
	
	
  return 0;
}
