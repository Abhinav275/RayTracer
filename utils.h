#include <iostream>
#include <fstream>
#include <sstream>
#include "models.h"
using namespace std;

// function to get tokens from the given string and delimiter
vector<string> getTokens(string line, char delimiter);

// Utility function to check given string is a number
bool checkNumber(string num);

// Utility function to check given string is a float
bool checkFloat(string num);

// function to get the output filename from the input filename
string getFilename(char* inputFile);

// function to write image headers in output ASCII-Image
// takes as input all the header parameters
void writeImageHeaders(string file, string imageType, string comments, ImageParameters& id, int colorRange);

// Utility function to debug
void debug(string s);

// Utility function to print integer
void debug(int n);

// Utility function to print float
void debug(float f);