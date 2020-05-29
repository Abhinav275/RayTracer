#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#ifndef UTILS_H
#define UTILS_H

// function to get tokens from the given string and delimiter
vector<string> getTokens(string line, char delimiter);

// Utility function to check given string is a number
bool checkNumber(string num);

// Utility function to check given string is a float
bool checkFloat(string num);

// function to get the output filename from the input filename
string getFilename(char* inputFile);

// Utility function to debug
void debug(string s);

// Utility function to print integer
void debug(int n);

// Utility function to print float
void debug(float f);

#endif