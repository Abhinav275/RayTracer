#include "rayCaster.h"
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

void debug(string s){
	cout<<s<<endl;
	return;
}

void debug(int n){
	cout<<n<<endl;
	return;
}

void debug(float f){
	cout<<f<<endl;
	return;
}

// Utility function to check given string is a number
bool checkNumber(string num){
	if(num[0]=='-' || num[0]=='+') num=num.substr(1, num.size()-1);
	for(auto i: num) if(!isdigit(i)) return false;
	return true;
}

// Utility function to check given string is a float
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

// function to get the output filename from the input filename
string getFilename(char* inputFile){
	vector<string> tokens = getTokens(string(inputFile), '.');
	if(tokens.empty()) return "-1";
	else return tokens[0]+".ppm";
}

// function to write image headers in output ASCII-Image
// takes as input all the header parameters
void writeImageHeaders(string file, string imageType, string comments, ImageParameters& id, int colorRange){
	// open output file
	ofstream myfile(file, std::ofstream::out);
	
	// write headers to the output file
	if(myfile.is_open()){
		myfile<<imageType<<"\n";
		myfile<<comments<<"\n";
		myfile<<id.dim.width<<" "<<id.dim.height<<"\n";
		myfile<<colorRange<<"\n";
	}

	// close output file
	myfile.close();
	return;
}

Texture readTextureFile(string filename){

	// open texture file
	ifstream inputFile(filename);

	if(inputFile.is_open()){
		string line;

		getline(inputFile, line);
		

		vector<string> headers = getTokens(line, ' ');

		if(!checkNumber(headers[1]) || !checkNumber(headers[2]) || !checkNumber(headers[3])) throw -1;

		int width = stoi(headers[1]);
		int height = stoi(headers[2]);
		int maxColor = stoi(headers[3]);

		vector<ColorType> result(height*width, {0.0,0.0,0.0});

		int colorListIndex = 0;
		int colorIndex = 0;
		int limit = width*height;

		while(colorListIndex<limit && getline(inputFile, line)){
			vector<string> tokens = getTokens(line, ' ');
			for(auto color: tokens){
				switch(colorIndex%3){
					case 0: {
						result[colorListIndex].R = (float)stoi(color)/(float)maxColor;
						colorIndex++;
						break;
					}
					case 1: {
						result[colorListIndex].G = (float)stoi(color)/(float)maxColor;
						colorIndex++;
						break; 
					}
					case 2: {
						result[colorListIndex].B = (float)stoi(color)/(float)maxColor;
						colorIndex++;
						colorListIndex++;
						break;
					}
				}
			}
		}


		inputFile.close();

		vector<vector<ColorType>> textureList(height, vector<ColorType>(width, {0.0,0.0,0.0}));

		colorListIndex = 0;

		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				textureList[i][j] = result[colorListIndex];
				colorListIndex++;
			}
		}

		Texture tex;
		tex.textureList = textureList;
		tex.width = width;
		tex.height = height;

		return tex;
	}

	else throw -1;

	inputFile.close();

	return {};
}

// Function to write the RGB pixel data to output image
void writeImageData(string file, ImageParameters& id, vector<vector<ColorType>> image){
	// open output file
	ofstream myfile(file, std::ofstream::out | std::ofstream::app);
	
	// traverse the pattern and write data to the output file
	for(int i=0;i<id.dim.height;i++){
		for(int j=0;j<id.dim.width;j++){
			myfile<<(int)(255.0*image[i][j].R)<<" "<<(int)(255.0*image[i][j].G)<<" "<<(int)(255.0*image[i][j].B)<<"\n";
		}
	}

	// close the image file
	myfile.close();
}

// Function to read input from the given filename 
ImageParameters readInput(char* filename){

	// open input file 
	ifstream inputFile(filename);

	// a map to keep track of required read values
	unordered_map<string, bool> headerMap = {
		{"eye", false},
		{"viewdir", false},
		{"updir", false},
		{"hfov", false},
		{"imsize", false},
		{"bkgcolor", false},
		{"mtlcolor", false},
		{"sphere", false},
		{"v", false},
		{"f", false},
		{"vn", false},
		{"vt", false},
		{"light", false},
		{"texture", false}
	};

	// a map between the switch case and input keywords
	unordered_map<string, int> headers = {
		{"eye", 0},
		{"viewdir", 1},
		{"updir", 2},
		{"hfov", 3},
		{"imsize", 4},
		{"bkgcolor", 5},
		{"mtlcolor", 6},
		{"sphere", 7},
		{"light", 8},
		{"attlight", 9},
		{"depthcueing", 10},
		{"v", 11},
		{"f", 12},
		{"vn", 13},
		{"vt", 14},
		{"texture", 15}
	};

	// an empty material to use for spheres
	Material mtr;
	Texture tex;

	ImageParameters id;
	id.depthFlag = false;
	
	// checking if input file is open
	if(inputFile.is_open()){
		string line;

		// read lines from input file one by one
		while(getline(inputFile, line)){

			// get tokens from file separated by space
			vector<string> tokens = getTokens(line, ' ');

			// ignore is empty line
			if(tokens.size()==0) continue;

			// switch case on keyword
			switch(headers[tokens[0]]){
				case 4:{

					// handling keyword imsize
					if(tokens.size()!=3 || !checkNumber(tokens[1]) || !checkNumber(tokens[2])) throw -1;
					int width = stoi(tokens[1]);
					int height = stoi(tokens[2]);

					if(width<=0 || height<=0) throw -1;
					
					ImageDimension dim = {width, height};
					id.dim = dim;
					headerMap["imsize"] = true;
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 0:{

					// handling keyword eye
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float x = stof(tokens[1]);
					float y = stof(tokens[2]);
					float z = stof(tokens[3]);

					Point eye = {(float)x, (float)y, (float)z};
					id.eye = eye;
					headerMap["eye"] = true;
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 1:{

					// handling keyword viewdir
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float dx = stof(tokens[1]);
					float dy = stof(tokens[2]);
					float dz = stof(tokens[3]);

					Vector viewdir = {(float)dx, (float)dy, (float)dz};
					id.viewdir = viewdir;
					headerMap["viewdir"] = true;
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 2:{

					// handling keyword updir
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float dx = stof(tokens[1]);
					float dy = stof(tokens[2]);
					float dz = stof(tokens[3]);

					Vector up = {(float)dx, (float)dy, (float)dz};
					id.up = up;
					headerMap["updir"] = true;
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 3:{

					// handling keyword hfov
					if(tokens.size()!=2 || !checkFloat(tokens[1])) throw -1;
					float angle = stof(tokens[1]);
					id.hfov = angle;
					headerMap["hfov"] = true;
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 5:{

					// hanlding keyword bkgcolor
					if(tokens.size()!=4 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;
					float r = stof(tokens[1]);
					float g = stof(tokens[2]);
					float b = stof(tokens[3]);

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0) throw -1;
					headerMap["bkgcolor"] = true;
					ColorType bkcolor = {r,g,b};
					id.bkgcolor = bkcolor;
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 6:{

					// hangling keyword mtlcolor
					if(tokens.size()!=11 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) ||
						!checkFloat(tokens[4]) || !checkFloat(tokens[5]) || !checkFloat(tokens[6]) ||
						!checkFloat(tokens[7]) || !checkFloat(tokens[8]) || !checkFloat(tokens[9]) ||
						!checkFloat(tokens[10])) throw -1;
					float r = stof(tokens[1]);
					float g = stof(tokens[2]);
					float b = stof(tokens[3]);

					float r1 = stof(tokens[4]);
					float g1 = stof(tokens[5]);
					float b1 = stof(tokens[6]);

					float ka = stof(tokens[7]);
					float kd = stof(tokens[8]);
 					float ks = stof(tokens[9]);

 					float n = stof(tokens[10]);


					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0) throw -1;
					if(r1<0.0 || r1>1.0 || g1<0.0 || g1>1.0 || b1<0.0 || b1>1.0) throw -1;
					headerMap["mtlcolor"] = true;
					ColorType mtlcolor = {(float)r, (float)g, (float)b};
					ColorType speccolor = {(float)r1, (float)g1, (float)b1};
					mtr = {mtlcolor, speccolor, ka, kd, ks, n};
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 7:{

					// hanlding keyword sphere
					if(tokens.size()!=5 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) || !checkFloat(tokens[4])) throw -1;
					float cx = stof(tokens[1]);
					float cy = stof(tokens[2]);
					float cz = stof(tokens[3]);
					float radius = stof(tokens[4]);

					if(radius<=0 || !headerMap["mtlcolor"]) throw -1;
					headerMap["sphere"] = true;

					SphereType sphere = {(float)cx, (float)cy, (float)cz, (float)radius, mtr};

					if(headerMap["texture"] == true) {
						sphere.textureFlag = true;
						sphere.tex = tex;
					}

					id.spheres.push_back(sphere);
					// cout<<headers[tokens[0]]<<endl;
					break;
				}
				case 8:{
 					
					// parse light
					if(tokens.size()!=8 || 
						!checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) ||
						!checkFloat(tokens[4]) ||
						!checkFloat(tokens[5]) || !checkFloat(tokens[6]) || !checkFloat(tokens[7])) throw -1;
					float x = stof(tokens[1]), y = stof(tokens[2]), z = stof(tokens[3]); 
					float r = stof(tokens[5]), g = stof(tokens[6]), b = stof(tokens[7]);
					float w = stof(tokens[4]);

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0 || (w!=1.0 && w!=0.0)) throw -1;
					headerMap["light"] = true;

					ColorType lightIntensity = {(float)r, (float)g, (float)b};
					LightSource l = {x,y,z,w,lightIntensity, 0, 0.0, 0.0, 0.0};
					id.lights.push_back(l);
					// cout<<headers[tokens[0]]<<endl;
					break;
				} case 9:{

					// parse attenuate light
					if(tokens.size()!=11 || 
						!checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) ||
						!checkFloat(tokens[4]) ||
						!checkFloat(tokens[5]) || !checkFloat(tokens[6]) || !checkFloat(tokens[7]) ||
						!checkFloat(tokens[8]) || !checkFloat(tokens[9]) || !checkFloat(tokens[10])) throw -1;
					float x = stof(tokens[1]), y = stof(tokens[2]), z = stof(tokens[3]); 
					float r = stof(tokens[5]), g = stof(tokens[6]), b = stof(tokens[7]);
					float w = stof(tokens[4]);
					float c1 = stof(tokens[8]), c2 = stof(tokens[9]), c3 = stof(tokens[10]);

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0 || (w!=1.0)) throw -1;
					headerMap["light"] = true;

					ColorType lightIntensity = {(float)r, (float)g, (float)b};
					LightSource l = {x,y,z,w,lightIntensity, 1, c1, c2 ,c3};
					id.lights.push_back(l);
					// cout<<headers[tokens[0]]<<endl;
					break;
				} case 10:{

					// input for depth cue
					if(tokens.size()!=8 ||
						!checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) ||
						!checkFloat(tokens[4]) || !checkFloat(tokens[5]) || 
						!checkFloat(tokens[6]) || !checkFloat(tokens[7])) throw -1;

					float r = stof(tokens[1]), g = stof(tokens[2]), b = stof(tokens[3]);
					float amax = stof(tokens[4]), amin = stof(tokens[5]);
					float dmax = stof(tokens[6]), dmin = stof(tokens[7]);

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0) throw -1;

					DepthCue depthCue = {r, g, b, amax, amin, dmax, dmin};
					id.depthCue = depthCue;
					id.depthFlag = true;
					// cout<<headers[tokens[0]]<<endl;
					break;
				} case 11:{

					// vertex point as input
					if(tokens.size()!= 4 ||
						!checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;

					float x = stof(tokens[1]), y = stof(tokens[2]), z = stof(tokens[3]);
					headerMap["v"] = true;
					Point v = {x,y,z};
					id.vertices.push_back(v);
					// cout<<headers[tokens[0]]<<endl;
					break; 
				} case 12:{

					// take care of faces
					if(tokens.size()!= 4) throw -1;

					vector<string> subTokens1 = getTokens(tokens[1], '/');
					vector<string> subTokens2 = getTokens(tokens[2], '/');
					vector<string> subTokens3 = getTokens(tokens[3], '/');

					int tokenSize = subTokens1.size();
					if(subTokens2.size()!=tokenSize || subTokens3.size()!=tokenSize) throw -1;
					
					if(!checkNumber(subTokens1[0]) || !checkNumber(subTokens2[0]) || !checkNumber(subTokens3[0])) throw -1;
					int v1 = stoi(subTokens1[0]), v2 = stoi(subTokens2[0]), v3 = stoi(subTokens3[0]);
					if(id.vertices.size()< max(max(v1, v2), v3) || v1<=0 || v2<=0 || v3<=0 || !headerMap["mtlcolor"]) throw -1;
					
					Triangle f;

					f.v1 = id.vertices[v1-1];
					f.v2 = id.vertices[v2-1];
					f.v3 = id.vertices[v3-1];

					if(tokenSize==2){
						if(subTokens1[1]=="" || subTokens2[1]=="" || subTokens3[1]=="" ||
							!checkNumber(subTokens1[1]) || !checkNumber(subTokens2[1]) || !checkNumber(subTokens3[1])) throw -1;
						
						float vt1 = stoi(subTokens1[1]);
						float vt2 = stoi(subTokens2[1]);
						float vt3 = stoi(subTokens3[1]);
						
						if(id.verticeTextures.size()< max(max(vt1, vt2), vt3) || vt1<=0 || vt2<=0 || vt3<=0 || !headerMap["texture"]) throw -1;
						
						f.vt1 = id.verticeTextures[vt1-1];
						f.vt2 = id.verticeTextures[vt2-1];
						f.vt3 = id.verticeTextures[vt3-1];
						f.textureFlag = true;
						f.tex = tex;

					}

					else if(tokenSize == 3){

						if(subTokens1[2]=="" || subTokens2[2]=="" || subTokens3[2]=="" ||
							!checkNumber(subTokens1[2]) || !checkNumber(subTokens2[2]) || !checkNumber(subTokens3[2])) throw -1;
						
						float vn1 = stoi(subTokens1[2]);
						float vn2 = stoi(subTokens2[2]);
						float vn3 = stoi(subTokens3[2]);
						
						if(id.verticeNormals.size()< max(max(vn1, vn2), vn3) || vn1<=0 || vn2<=0 || vn3<=0) throw -1;

						f.vn1 = id.verticeNormals[vn1-1];
						f.vn2 = id.verticeNormals[vn2-1];
						f.vn3 = id.verticeNormals[vn3-1];
						f.normalFlag = true;


						if(subTokens1[1]!="" || subTokens2[1]!="" || subTokens3[1]!=""){
							if(subTokens1[1]=="" || subTokens2[1]=="" || subTokens3[1]=="" ||
							!checkNumber(subTokens1[1]) || !checkNumber(subTokens2[1]) || !checkNumber(subTokens3[1])) throw -1;

							float vt1 = stoi(subTokens1[1]);
							float vt2 = stoi(subTokens2[1]);
							float vt3 = stoi(subTokens3[1]);
							
							if(id.verticeTextures.size()< max(max(vt1, vt2), vt3) || vt1<=0 || vt2<=0 || vt3<=0 || !headerMap["mtlcolor"]) throw -1;
							
							f.vt1 = id.verticeTextures[vt1-1];
							f.vt2 = id.verticeTextures[vt2-1];
							f.vt3 = id.verticeTextures[vt3-1];
							f.textureFlag = true;
							f.tex = tex;
						}
					}

					f.mtr = mtr;

					headerMap["f"] = true;
					id.triangles.push_back(f);
					// cout<<headers[tokens[0]]<<endl;
					break;
				} case 13:{
					// vertex point as input
					if(tokens.size()!= 4 ||
						!checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3])) throw -1;

					float x = stof(tokens[1]), y = stof(tokens[2]), z = stof(tokens[3]);
					headerMap["vn"] = true;
					Vector v = {x,y,z};
					id.verticeNormals.push_back(v);
					// cout<<headers[tokens[0]]<<endl;
					break; 
				} case 14:{
					// vertex point as input
					if(tokens.size()!= 3 ||
						!checkFloat(tokens[1]) || !checkFloat(tokens[2])) throw -1;

					float x = stof(tokens[1]), y = stof(tokens[2]);
					headerMap["vt"] = true;
					Point v = {x,y};
					id.verticeTextures.push_back(v);
					// cout<<headers[tokens[0]]<<endl;
					break; 
				} case 15:{

					// texture file name as input
					if(tokens.size()!= 2) throw -1;
					tex = readTextureFile(tokens[1]);
					headerMap["texture"] = true;

					break;
				}
				default:

					// default case throwing error
					throw -1;
			}

		};

		// checking required keywords have been read
		for(auto& i: headerMap){
			if(i.second == false && (i.first != "mtlcolor" && i.first != "sphere" 
				&& i.first != "vn" && i.first != "vt" && i.first != "texture"
				&& i.first != "f" && i.first != "v")) throw -1;
		}

		cout<<"Successfully read the input file."<<endl;
	}

	else throw -1;
	//close the input file
	inputFile.close();

	// return image parameters
	return id;

}

// Utility function to print Vector
void printVector(Vector a){
	cout<<a.dx<<","<<a.dy<<","<<a.dz<<","<<endl;
	return;
}

// Utility function to print Point
void printPoint(Point a){
	cout<<a.x<<","<<a.y<<","<<a.z<<","<<endl;
	return;
}

// Utility function to print a RayType
void printRay(RayType a){
	cout<<a.x<<","<<a.y<<","<<a.z<<","<<a.dx<<","<<a.dy<<","<<a.dz<<","<<endl;
	return;
}

// Function to initialize the image
vector<vector<ColorType>> initializeImage(ImageParameters& id){
	int width = id.dim.width;
	int height = id.dim.height;

	vector<vector<ColorType>> image(height, vector<ColorType>(width, {0.0,0.0,0.0}));
	return image;
}

bool equalVector(Vector a, Vector b){
	return ((a.dx == b.dx) && (a.dy == b.dy) && (a.dz == b.dz)); 
}

float getMagnitude(Vector a){
	float magnitude = sqrt(a.dx*a.dx + a.dy*a.dy + a.dz*a.dz);
	return magnitude;
}

// Function to get normalized unit Vector
Vector normalize(Vector a){
	float magnitude = getMagnitude(a);
	if (magnitude == 0.0) throw -2; 
	Vector result;
	result.dx = a.dx/magnitude;
	result.dy = a.dy/magnitude;
	result.dz = a.dz/magnitude;
	return result;
}


// Function to add two given Vectors
Vector add(Vector a, Vector b){
	Vector result;
	result.dx = a.dx+b.dx;
	result.dy = a.dy+b.dy;
	result.dz = a.dz+b.dz;
	return result;
}

// Function to add a point and a Vector
Vector add(Point a, Vector b){
	Vector result;
	result.dx = a.x+b.dx;
	result.dy = a.y+b.dy;
	result.dz = a.z+b.dz;
	return result;
}

// Function to get a vector by taking difference of a and b 
Vector getVector(Point a, Point b){
	Vector result;
	result.dx = -a.x+b.x;
	result.dy = -a.y+b.y;
	result.dz = -a.z+b.z;
	return result;
}

ColorType add(ColorType a, ColorType b){
	ColorType result;
	result.R = a.R+b.R;
	result.G = a.G+b.G;
	result.B = a.B+b.B;
	return result;
}

// Function to to multiply given Vector with D ie viewing distance
Vector multiplyD(Vector a){
	Vector result = {D*a.dx, D*a.dy ,D*a.dz};
	return result;
}

// Function to multiply vector with scalar
Vector multiplyScalar(Vector a, float b){
	Vector result = {b*a.dx, b*a.dy ,b*a.dz};
	return result; 
}

ColorType multiplyScalar(ColorType c, float s){
	return {c.R*s, c.G*s, c.B*s};
}

// Function to get negative of a given Vector
Vector negateVector(Vector a){
	Vector result = {-a.dx, -a.dy ,-a.dz};
	return result;
}

// Function to get cross products of 2 Vectors
Vector crossProduct(Vector a, Vector b){
	try{
		if(equalVector(normalize(a), normalize(b)) || equalVector(normalize(a), negateVector(normalize(b)) )) throw -2;
		Vector result;
		result.dz = a.dx*b.dy - a.dy*b.dx; 
		result.dx= a.dy*b.dz - a.dz*b.dy; 
		result.dy = a.dz*b.dx - a.dx*b.dz;
		return result;
	} catch (int e) {
		throw e;
	}

}

float getPlainConstant(Vector n, Point p){
	return -(n.dx*p.x + n.dy*p.y + n.dz*p.z);
}

// dot product of two vectors
float dotProduct(Vector a, Vector b){
	float result = a.dx*b.dx + a.dy*b.dy + a.dz*b.dz;
	return result;
}

// add colors elementwise
ColorType elementMultiply(ColorType a, ColorType b){
	ColorType result = {a.R*b.R, a.G*b.G, a.B*b.B};
	return result;
}


// Function to get image plane Vectors ie u, v and w 
void getImagePlaneVectors(ImageParameters& id){

	try {	
		// taking negative of viewdir and normalizing it
		id.w = {-id.viewdir.dx, -id.viewdir.dy, -id.viewdir.dz};
		id.w = normalize(id.w);

		// taking cross product of viewdir and updir
		id.u = crossProduct(id.viewdir, id.up);
		id.u = normalize(id.u);

		// taking cross product of u and viewdir
		id.v = crossProduct(id.u, id.viewdir);
		id.v = normalize(id.v);
	} catch (int e){
		throw e;
	}
	
	return;
}

// Function to get the viewing window parameters
void getImageViewingWindow(ImageParameters& id){

	try{
		// calculate the width and height of the viewing window
		float width = 2*D*tan((id.hfov/2.0)*(PI/180.0));
		float height = width / ((float)id.dim.width/(float)id.dim.height);

		ViewingWindow vw;

		Vector viewOrigin = add(id.eye, multiplyD(normalize(id.viewdir)) );

		// calculate corners of the viewing window
		vw.ul = add( viewOrigin , add( multiplyScalar(id.u, -width/2.0) , multiplyScalar(id.v, height/2.0) ) ); 
		vw.ur = add( viewOrigin , add( multiplyScalar(id.u, width/2.0) , multiplyScalar(id.v, height/2.0) ) ); 
		vw.ll = add( viewOrigin , add( multiplyScalar(id.u, -width/2.0) , multiplyScalar(id.v, -height/2.0) ) ); 
		vw.lr = add( viewOrigin , add( multiplyScalar(id.u, width/2.0) , multiplyScalar(id.v, -height/2.0) ) ); 

		id.vw = vw;

	} catch (int e){
		throw e;
	}

	return;
}

// Utility function to make a ray given a Point and a Vector
RayType makeRay(Point p, Vector a){
	try{
		Vector direction = negateVector(add(p, negateVector(a)));
		direction = normalize(direction);

		RayType result = {p.x, p.y, p.z, direction.dx, direction.dy, direction.dz};
		return result;
	} catch (int e){
		throw e;
	}
}

RayType makeRay(Vector p, Vector a){
	Point pt = {p.dx, p.dy, p.dz};
	return makeRay(pt, a);
}

// function to make ray if direction is given
RayType makeDirRay(Vector p, Vector direction){
	direction = normalize(direction);
	RayType result = {p.dx, p.dy, p.dz, direction.dx, direction.dy, direction.dz};
	return result;
}

// get intersection point
Vector getRayPoint(RayType ray, float t){
	return {ray.x+t*ray.dx, ray.y+t*ray.dy, ray.z+t*ray.dz};
}

// Function to get all the rays passing through the viewing window
vector<vector<RayType>> getRays(ImageParameters& id){
	int width = id.dim.width;
	int height = id.dim.height;

	vector<vector<RayType>> result;

	// Calculate delta_ch and delta_cv
	Vector delta_ch = multiplyScalar( add(id.vw.ur, negateVector(id.vw.ul)) , 1.0/(2.0*(float)width));
	Vector delta_cv = multiplyScalar( add(id.vw.ll, negateVector(id.vw.ul)) , 1.0/(2.0*(float)height));

	// Add them to upper left point of the viewing window
	Vector delta_center = add(id.vw.ul, add(delta_ch, delta_cv));

	// calculate delta_h and delta_v ie steps for the iterator
	Vector delta_h = multiplyScalar( add(id.vw.ur, negateVector(id.vw.ul)) , 1.0/((float)width));
	Vector delta_v = multiplyScalar( add(id.vw.ll, negateVector(id.vw.ul)) , 1.0/((float)height));

	// Iterate through the viewing window and create rays
	for(int i=0;i<height;i++){
		result.push_back({});
		for(int j=0;j<width;j++){

			// temp = ul + (i)delta_v + (j)delta_h + delta_ch + delta_cv
			Vector temp = add(delta_center, add( multiplyScalar(delta_v, i), multiplyScalar(delta_h, j)));

			// make ray from eye to temp and keep it in result list
			RayType r = makeRay(id.eye, temp);
			result[i].push_back(r);
		}
	}

	// return list of rays
	return result;
}

float getTriangleArea(Point p0, Point p1, Point p2){
	try{
		Vector e1 = getVector(p0, p1);
		Vector e2 = getVector(p0, p2);

		return 0.5*(getMagnitude(crossProduct(e1,e2)));
	} catch (int e){
		return FLT_MAX;
	}
}

// Function to check if a shpere and a ray interset
float findSphereIntersectionDistance(RayType ray, SphereType sphere){
	
	// find A, B, C of the quadtratic equation
	float A = 1.0;
	float B = 2.0 * ( ray.dx * (ray.x - sphere.cx) +  ray.dy * (ray.y - sphere.cy) + ray.dz * (ray.z - sphere.cz));
	float C = (ray.x - sphere.cx)*(ray.x - sphere.cx) + (ray.y - sphere.cy)*(ray.y - sphere.cy) + (ray.z - sphere.cz)*(ray.z - sphere.cz) - sphere.r*sphere.r;

	// cBeck if solution exists
	float mod = B*B - 4.0*A*C;
	if(mod<0) return FLT_MAX;
	else if(mod==0) return -B/(2.0*A);
	else {
		float t1,t2;
		t1 = (-B+sqrt(mod))/(2.0*A);
		t2 = (-B-sqrt(mod))/(2.0*A);

		// check that the distance is positive
		if(t1>0.0 && t2>0.0) return min(t1,t2);
		else if(t1>0.0) return t1;
		else if(t2>0.0) return t2;
		else return FLT_MAX;
	}
}

vector<float> getBaricenterCoordiantes(Triangle t, Point p){
	float area = getTriangleArea(t.v1, t.v2, t.v3);

	float beta = getTriangleArea(t.v1, p, t.v3) / area;
	float gamma = getTriangleArea(t.v1, t.v2, p) / area;

	float alpha = getTriangleArea(p, t.v2, t.v3) / area;

	return {alpha, beta, gamma};
}

float findTriangleIntersectionDistance(RayType ray, Triangle t){
	Vector e1 = getVector(t.v1, t.v2);
	Vector e2 = getVector(t.v1, t.v3);
	Vector n = crossProduct(e1,e2);

	float plainConstant = getPlainConstant(n, t.v1);

	float denominator = n.dx*ray.dx + n.dy*ray.dy + n.dz*ray.dz;

	if(denominator == 0) return FLT_MAX;

	float numerator = -(n.dx*ray.x + n.dy*ray.y + n.dz*ray.z + plainConstant);
	float distance = numerator/denominator; 

	if(distance<0.0) return FLT_MAX;
	
	Point p = {ray.x + distance*ray.dx, ray.y + distance*ray.dy, ray.z + distance*ray.dz};
	// printPoint(p);

	vector<float> coordinates = getBaricenterCoordiantes(t, p);

	if(coordinates[0]>1.0 || coordinates[1]>1.0 || coordinates[2]>1.0 || (coordinates[0] + coordinates[1]+ coordinates[2])!=1.0) return FLT_MAX;

	return distance;

}

// function to get the attenuation factor
float getAttFactor(float c1, float c2, float c3, float distance){
	return (float)1.0/(c1+ c2*distance + c3*distance*distance);
}

// function to get depth cue factor
float getDepthAlpha(DepthCue depthCue, float distance){
	if(distance <= depthCue.dmin) return depthCue.amax;
	else if(distance >= depthCue.dmax) return depthCue.amin;
	else{
		return depthCue.amin + (depthCue.amax - depthCue.amin)*(depthCue.dmax - distance)/(depthCue.dmax - depthCue.dmin);
	}
}

// Function to return the color of the intersection point
ColorType shadeRay(ImageParameters& id, int objectId, int objectType, Vector pointOfIntersection){

	// initialize color variables
	ColorType oA, oD = {0.0, 0.0, 0.0}, oS = {0.0, 0.0, 0.0}, sigma={0.0, 0.0, 0.0};
	Vector normal;
	Material objectMat;

	if(objectType == 0){
		// throw shadow rays from the intersection point
		SphereType object = id.spheres[objectId];
		oA = multiplyScalar(object.mtr.materialColor, object.mtr.ka);
		normal = add(negateVector({object.cx, object.cy, object.cz}), pointOfIntersection);
		objectMat = object.mtr;
		if(object.textureFlag == true){
			Vector n = normalize(normal);
			float v = acos(n.dz)/PI;
			float theta = atan2(n.dy, n.dx);
			if(theta < 0.0) theta = theta + 2*PI;

			float u = theta/(2*PI);

			int i = (int)(v*(float)(object.tex.height-1));
			int j = (int)(u*(float)(object.tex.width-1));

			objectMat.materialColor = object.tex.textureList[i][j];

		}
	}

	else if(objectType == 1){
		// throw shadow rays from the intersection point
		Triangle object = id.triangles[objectId];
		oA = multiplyScalar(object.mtr.materialColor, object.mtr.ka);
		Vector e1 = getVector(object.v1, object.v2);
		Vector e2 = getVector(object.v1, object.v3);
		// cout<<object.normalFlag<<endl;
		vector<float> coordinates = getBaricenterCoordiantes(object, {pointOfIntersection.dx, pointOfIntersection.dy, pointOfIntersection.dz});
		if(object.normalFlag != true) normal = crossProduct(e1,e2);
		else{
			normal = add(add(multiplyScalar(object.vn1, coordinates[0]), multiplyScalar(object.vn2, coordinates[1])),
							multiplyScalar(object.vn3, coordinates[2]));
		}

		objectMat = object.mtr;

		if(object.textureFlag == true){
			float u = coordinates[0]*object.vt1.x + coordinates[1]*object.vt2.x + coordinates[2]*object.vt3.x;
			float v = coordinates[0]*object.vt1.y + coordinates[1]*object.vt2.y + coordinates[2]*object.vt3.y;

			int i = (int)(v*(float)(object.tex.height));
			int j = (int)(u*(float)(object.tex.width));

			objectMat.materialColor = object.tex.textureList[i][j];

		}
	}


	// ger normal vector from the point of intersection
	normal = normalize(normal);

	// get eye position vector
	Vector eyePosition = {id.eye.x, id.eye.y, id.eye.z};
	Vector V = add(eyePosition, negateVector(pointOfIntersection));


	ColorType res = oA; 
	
	// traversing all the light sources
	for(auto& lightSource: id.lights){

		float shadowFlag=0.0;
		
		// getting the Light source vector
		Vector lightSourceVector = {lightSource.x, lightSource.y, lightSource.z};
		lightSourceVector = negateVector(lightSourceVector);


		// forming multiple shadow rays for making soft shadows
		for(int x = 0; x < SHADOWTESTTIMES; x++){

			// find jitter and add it to ray input
			float jitter_x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float jitter_y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float jitter_z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			jitter_x = jitter_x*0.5;
			jitter_y = jitter_y*0.5;
			jitter_z = jitter_z*0.5;

			// make shadow rays
			RayType shadowRay = makeRay(pointOfIntersection, {lightSource.x+jitter_x, lightSource.y+jitter_y, lightSource.z+jitter_z});
			if(lightSource.w == 0.0) shadowRay = makeDirRay(pointOfIntersection, lightSourceVector);

			// find intersection of shadow rays with sphere
			float minDistance = FLT_MAX;
			for(int i=0;i<id.spheres.size();i++){
				
				if(i == objectId) continue;

				float dist = findSphereIntersectionDistance(shadowRay, id.spheres[i]);
				if(dist == FLT_MAX) continue;

				else if(minDistance > dist && dist > EPI){
					minDistance = dist;
				}
			}

			//  calculate shadow flag
			if(lightSource.w == 0.0 && minDistance!=FLT_MAX) shadowFlag += 0.0;
			else if(lightSource.w==1.0 && minDistance!=FLT_MAX){
				if(minDistance < getMagnitude( add(pointOfIntersection, lightSourceVector))) shadowFlag += 0.0;
				else shadowFlag += 1.0;
			}
			else if(minDistance==FLT_MAX) shadowFlag += 1.0;
		}

		// normalize shadow flag
		shadowFlag = shadowFlag/(float)(SHADOWTESTTIMES);

		if(lightSource.w == 0.0 && shadowFlag<1.0) shadowFlag = 0.0;

		Vector L, H;
		
		// N.L dot product for diff factor
		if(lightSource.w == 1.0){
			L = add(negateVector(pointOfIntersection), negateVector(lightSourceVector));
		} else L = lightSourceVector;

		L = normalize(L);

		// find H and N.H for spectral factor
		H = add(L,normalize(V));
		H = normalize(H);
		

		float nDotL = max((float)0.0, dotProduct(normal, L));
		float nDotH = dotProduct(normal, H);
		nDotH = pow(nDotH, objectMat.n);
		nDotH = max((float)0.0, nDotH);

		// mulitply colors with constants
		oD = multiplyScalar(objectMat.materialColor, objectMat.kd);
		oD = multiplyScalar(oD, nDotL);

		oS = multiplyScalar(objectMat.specColor, objectMat.ks); 
		oS = multiplyScalar(oS, nDotH);

		// add the colors element wise
		ColorType c = elementMultiply(lightSource.c, add(oD, oS));

		// attenuate if required
		if(lightSource.attFlag == 1){
			float attFactor = getAttFactor(lightSource.c1, lightSource.c2, lightSource.c3, getMagnitude( add(pointOfIntersection, lightSourceVector))); 
			c = multiplyScalar(c, attFactor);
		}
		// multiple with shadow flag and add
		sigma = add(sigma, multiplyScalar(c, shadowFlag));
	}

	// add the sigma to ambia light
	res = add(res, sigma);

	// apply depth cueing
	if(id.depthFlag == true){

		// get distance and get getDepthAlpha
		float distance = getMagnitude(V);
		float depthCueFactor = getDepthAlpha(id.depthCue, distance);
		res = add(multiplyScalar(res, depthCueFactor), multiplyScalar(id.depthCue.c, ((float)1.0-depthCueFactor)));
	}

	// clamp the colors
	res.R = min((float)1.0, res.R);
	res.G = min((float)1.0, res.G);
	res.B = min((float)1.0, res.B);
	return res;
}

// Function to trace ray and find intersections with given image parameters
ColorType traceRay(RayType ray, ImageParameters& id){
	int objectId = -1;
	int objectType = -1;
	float minDistance = FLT_MAX;
	
	// iterate over all image spheres
	for(int i=0;i<id.spheres.size();i++){

		// find if the object intersects
		float dist = findSphereIntersectionDistance(ray, id.spheres[i]);
		if(dist == FLT_MAX) continue;
		
		// check if the intersection point is closer than previous intersection point
		else if(minDistance > dist){
			minDistance = dist;
			objectId = i;
			objectType = 0;
		}
	}

	// iterate over all image triangle
	for(int i=0;i<id.triangles.size();i++){

		// find if the object intersects
		float dist = findTriangleIntersectionDistance(ray, id.triangles[i]);
		if(dist == FLT_MAX) continue;
		
		// check if the intersection point is closer than previous intersection point
		else if(minDistance > dist){
			minDistance = dist;
			objectId = i;
			objectType = 1;
		}
	}
	// return the color if there is an intersection point else return background color
	if(minDistance != FLT_MAX) return shadeRay(id,objectId, objectType, getRayPoint(ray, minDistance));
	return id.bkgcolor;
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
		vector<vector<RayType>> rays = getRays(id);


		// traceRay({0.0, 0.0, 5.0, 0.2, 0.2, -5.0}, id);
		// traceRay({0.0, 0.0, 5.0, -0.2, -0.2, -5.0}, id);
		// traceRay({0.0, 0.0, 5.0, 0.2, -0.2, -5.0}, id);
		// traceRay({0.0, 0.0, 5.0, -0.2, 0.2, -5.0}, id);

		for(int i=0;i<id.dim.height;i++){
			for(int j=0;j<id.dim.width;j++){
				image[i][j] = traceRay(rays[i][j], id);
			}
		}

		// get filename from 
		string outputFile = getFilename(argv[1]);

		cout<<"Image Filename: "<<outputFile<<endl;

		// write ASCII-image headers to the output file
		writeImageHeaders(outputFile, "P3", "#image with ray casting", id, 255);
		
		// write the image pixel data to the output file
		writeImageData(outputFile, id, image);

	} catch (int e){
		if(e==-1) cout<< "Unable to process input file. Kindly check the input file format."<<endl;
		else if(e==-2) cout<<"Viewdir and Updir vectors are parallel, kindly make them non parallel vectors."<<endl;
		return 0;
	}

	return 0;
}