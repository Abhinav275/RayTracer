#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "models.h"
#include "utils.h"
using namespace std;

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// function to write image headers in output ASCII-Image
// takes as input all the header parameters
void writeImageHeaders(string file, string imageType, string comments, ImageParameters& id, int colorRange);

// function to read texture files
Texture readTextureFile(string filename);

// Function to write the RGB pixel data to output image
void writeImageData(string file, ImageParameters& id, vector<vector<ColorType>> image);

// Function to read input from the given filename 
ImageParameters readInput(char* filename);

#endif