#include "homework1.h"
using namespace std;

// function to get tokens from the given string and delimiter
vector<string> getTokens(string line, char delimiter){
	stringstream lineStream(line);
	vector<string> tokens;
	string word;
	
	// traverse the line stream using delimiter
	while(getline(lineStream, word, delimiter)){
		tokens.push_back(word);
	}

	// return tokens vector
	return tokens;
}

// Utility function to check given string is a number
bool checkNumber(string num){
	if(num[0]=='-' || num[0]=='+') num=num.substr(1, num.size()-1);
	for(auto i: num) if(!isdigit(i)) return false;
	return true;
}

bool checkFloat(string num){
	int decimalCount=0;
	if(num[0]=='-' || num[0]=='+') num=num.substr(1, num.size()-1);
	for(auto i: num){
		if(i=='.') decimalCount++;
		else if(!isdigit(i)) return false;

		if(decimalCount>1) return false;
	}
	return true;
}

// function to get read the input file and get image dimensions
// pair<int, int> getDimensions(char* filename){

// 	// open input file 
// 	ifstream inputFile(filename);
// 	pair<int, int> dimensions;
// 	if(inputFile.is_open()){
// 		string line;

// 		// read first line from input file
// 		getline(inputFile, line);

// 		// get tokens from file separated by space
// 		vector<string> tokens = getTokens(line, ' ');

// 		// confirm the format of the input file
// 		if(tokens.size()!=3 
// 			|| tokens[0]!="imsize" 
// 			|| !checkNumber(tokens[1]) 
// 			|| !checkNumber(tokens[2])) return make_pair(-1,-1);
		
// 		// make a pair using width and height from the file
// 		dimensions = make_pair(stoi(tokens[1]), stoi(tokens[2]));
// 	}

// 	//close the input file
// 	inputFile.close();

// 	// return image dimensions as a pair of integers
// 	return dimensions;
// }

// function to get the output filename from the input filename
string getFilename(char* inputFile){
	vector<string> tokens = getTokens(string(inputFile), '.');
	if(tokens.empty()) return "-1";
	else return tokens[0]+".ppm";
}

// function to write image headers in output ASCII-Image
// takes as input all the header parameters
// void writeImageHeaders(string file, string imageType, string comments, pair<int, int> imageDimensions, int colorRange){
// 	// open output file
// 	ofstream myfile(file, std::ofstream::out);
	
// 	// write headers to the output file
// 	if(myfile.is_open()){
// 		myfile<<imageType<<"\n";
// 		myfile<<comments<<"\n";
// 		myfile<<imageDimensions.first<<" "<<imageDimensions.second<<"\n";
// 		myfile<<colorRange<<"\n";
// 	}

// 	// close output file
// 	myfile.close();
// 	return;
// }

// // function to write the RGB pixel data to output image
// void writeImageData(string file, pair<int, int> imageDimensions){
// 	// open output file
// 	ofstream myfile(file, std::ofstream::out | std::ofstream::app);
	
// 	// get RGB image pattern
// 	vector<vector<vector<int>>> pattern = getImagePattern(imageDimensions);
	
// 	// traverse the pattern and write data to the output file
// 	for(int i=0;i<imageDimensions.second;i++){
// 		for(int j=0;j<imageDimensions.first;j++){
// 			myfile<<pattern[i][j][0]<<" "<<pattern[i][j][1]<<" "<<pattern[i][j][2]<<"\n";
// 		}
// 	}

// 	// close the image file
// 	myfile.close();
// }

ImageParameters readInput(char* filename){

	// open input file 
	ifstream inputFile(filename);
	unordered_map<string, bool> headerMap = {
		{"eye", false},
		{"viewdir", false},
		{"updir", false},
		{"hfov", false},
		{"imsize", false},
		{"bkgcolor", false},
		{"mtlcolor", false},
		{"sphere", false}
	};

	unordered_map<string, int> headers = {
		{"eye", 0},
		{"viewdir", 1},
		{"updir", 2},
		{"hfov", 3},
		{"imsize", 4},
		{"bkgcolor", 5},
		{"mtlcolor", 6},
		{"sphere", 7}
	};

	Material mtr;

	ImageParameters id;
	
	if(inputFile.is_open()){
		string line;

		// read first line from input file
		while(getline(inputFile, line)){

			// get tokens from file separated by space
			vector<string> tokens = getTokens(line, ' ');

			if(tokens.size()==0) continue;

			switch(headers[tokens[0]]){
				case 4:{
					if(tokens.size()!=3 || !checkNumber(tokens[1]) || !checkNumber(tokens[2])) throw -1;
					int width = stoi(tokens[1]);
					int height = stoi(tokens[2]);

					if(width<=0 || height<=0) throw -1;
					
					ImageDimension dim = {width, height};
					id.dim = dim;
					headerMap["imsize"] = true;
					break;
				}
				case 0:{
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float x = stof(tokens[1]);
					float y = stof(tokens[2]);
					float z = stof(tokens[3]);

					Point eye = {(float)x, (float)y, (float)z};
					id.eye = eye;
					headerMap["eye"] = true;
					break;
				}
				case 1:{
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float dx = stof(tokens[1]);
					float dy = stof(tokens[2]);
					float dz = stof(tokens[3]);

					Vector viewdir = {(float)dx, (float)dy, (float)dz};
					id.viewdir = viewdir;
					headerMap["viewdir"] = true;
					break;
				}
				case 2:{
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float dx = stof(tokens[1]);
					float dy = stof(tokens[2]);
					float dz = stof(tokens[3]);

					Vector up = {(float)dx, (float)dy, (float)dz};
					id.up = up;
					headerMap["updir"] = true;
					break;
				}
				case 3:{
					if(tokens.size()!=2 || !checkFloat(tokens[1])) throw -1;
					float angle = stof(tokens[1]);
					id.hfov = angle;
					headerMap["hfov"] = true;
					break;
				}
				case 5:{
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float r = stof(tokens[1]);
					float g = stof(tokens[2]);
					float b = stof(tokens[3]);

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0) throw -1;
					headerMap["bkgcolor"] = true;
					ColorType bkcolor = {r,g,b};
					id.bkgcolor = bkcolor;
					break;
				}
				case 6:{
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float r = stof(tokens[1]);
					float g = stof(tokens[2]);
					float b = stof(tokens[3]);

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0) throw -1;
					headerMap["mtlcolor"] = true;
					ColorType mtlcolor = {(float)r, (float)g, (float)b};
					mtr = {mtlcolor};
					break;
				}
				case 7:{
					if(tokens.size()!=5 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) || !checkFloat(tokens[4])) throw -1;
					float cx = stof(tokens[1]);
					float cy = stof(tokens[2]);
					float cz = stof(tokens[3]);
					float radius = stof(tokens[4]);

					if(radius<=0 || !headerMap["mtlcolor"]) throw -1;
					headerMap["sphere"] = true;

					SphereType sphere = {(float)cx, (float)cy, (float)cz, (float)radius, mtr};
					id.spheres.push_back(sphere);
					break;
				}
				default:
					throw -1;
			}

		};
			cout<<"Successfully read the input file"<<endl;
	}

	//close the input file
	inputFile.close();

	// return image dimensions as a pair of integers
	return id;

}

vector<vector<ColorType>> initializeImage(ImageParameters& id){
	int width = id.dim.width;
	int height = id.dim.height;

	vector<vector<ColorType>> image(height, vector<ColorType>(width, {0.0,0.0,0.0}));
	return image;
}

// get cross products of 2 Vectors
Vector crossProduct(Vector a, Vector b){
	Vector result;
	result.dz = a.dx*b.dy - a.dy*b.dx; 
	result.dx= a.dy*b.dz - a.dz*b.dy; 
	result.dy = a.dz*b.dx - a.dx*b.dz;
	return result; 
}

// get normalized unit Vector
Vector normalize(Vector a){
	float magnitude = sqrt(a.dx*a.dx + a.dy*a.dy + a.dz*a.dz);
	Vector result;
	result.dx = a.dx/magnitude;
	result.dy = a.dy/magnitude;
	result.dz = a.dz/magnitude;
	return result;
}

Vector add(Vector a, Vector b){
	Vector result;
	result.dx = a.dx+b.dx;
	result.dy = a.dy+b.dy;
	result.dz = a.dz+b.dz;
	return result;
}

Vector add(Point a, Vector b){
	Vector result;
	result.dx = a.x+b.dx;
	result.dy = a.y+b.dy;
	result.dz = a.z+b.dz;
	return result;
}

Vector multiplyD(Vector a){
	Vector result = {D*a.dx, D*a.dy ,D*a.dz};
	return result;
}

Vector multiplyScalar(Vector a, float b){
	Vector result = {b*a.dx, b*a.dy ,b*a.dz};
	return result; 
}

Vector negate(Vector a){
	Vector result = {-a.dx, -a.dy ,-a.dz};
	return result;
}

void printVector(Vector a){
	cout<<a.dx<<","<<a.dy<<","<<a.dz<<","<<endl;
	return;
}

void getImagePlaneVectors(ImageParameters& id){
	
	id.w = {-id.viewdir.dx, -id.viewdir.dy, -id.viewdir.dz};
	id.w = normalize(id.w);

	id.u = crossProduct(id.viewdir, id.up);
	id.u = normalize(id.u);

	id.v = crossProduct(id.u, id.viewdir);
	id.v = normalize(id.v);
	
	return;
}

void getImageViewingWindow(ImageParameters& id){
	float width = 2*D*tan((id.hfov/2.0)*(PI/180.0));
	float height = width / ((float)id.dim.width/(float)id.dim.height);

	ViewingWindow vw;

	Vector viewOrigin = add(id.eye, multiplyD(normalize(id.viewdir)) );

	vw.ul = add( viewOrigin , add( multiplyScalar(id.u, -width/2.0) , multiplyScalar(id.v, height/2.0) ) ); 
	vw.ur = add( viewOrigin , add( multiplyScalar(id.u, width/2.0) , multiplyScalar(id.v, height/2.0) ) ); 
	vw.ll = add( viewOrigin , add( multiplyScalar(id.u, -width/2.0) , multiplyScalar(id.v, -height/2.0) ) ); 
	vw.lr = add( viewOrigin , add( multiplyScalar(id.u, width/2.0) , multiplyScalar(id.v, -height/2.0) ) ); 

	id.vw = vw;

	return;
}

// main function to draw the image
int main(int argc, char** argv){

	// confirm the number of arguments are atleast 2
	if(argc < 2){
		cout<<"Please provide the filename"<<endl;
		return 0;
	}

	if(argc > 2){
		cout<<"Please provide only one filename"<<endl;
		return 0;
	}

	try{
	// read input file 	
		ImageParameters id = readInput(argv[1]);
		vector<vector<ColorType>> image = initializeImage(id);
		getImagePlaneVectors(id);
		getImageViewingWindow(id);
	} catch (int e){
		if(e==-1) cout<< "Unable to process input file. Kindly check the input file format."<<endl;
		return 0;
	}


	// // confirm that the image dimensions were read correctly
	// if(imageDimensions.first <= 0 || imageDimensions.second <= 0){
	// 	cout<< "Unable to process input file. Kindly check the input file format."<<endl;
	// 	return 0;
	// }

	// cout<<"Image dimensions: "<<imageDimensions.first<<" "<<imageDimensions.second<<endl;

	// // get filename from 
	// string outputFile = getFilename(argv[1]);

	// cout<<"Image Filename: "<<outputFile<<endl;

	// // write ASCII-image headers to the output file
	// writeImageHeaders(outputFile, "P3", "#image with dual gradient", imageDimensions, 255);
	
	// // write the image pixel data to the output file
	// writeImageData(outputFile, imageDimensions);
	return 0;
}