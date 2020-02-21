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
void writeImageHeaders(string file, string imageType, string comments, ImageParameters id, int colorRange){
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

// Function to write the RGB pixel data to output image
void writeImageData(string file, ImageParameters id, vector<vector<ColorType>> image){
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
		{"sphere", false}
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
		{"sphere", 7}
	};

	// an empty material to use for spheres
	Material mtr;

	ImageParameters id;
	
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
					break;
				}
				case 3:{

					// handling keyword hfov
					if(tokens.size()!=2 || !checkFloat(tokens[1])) throw -1;
					float angle = stof(tokens[1]);
					id.hfov = angle;
					headerMap["hfov"] = true;
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
					break;
				}
				case 6:{

					// hangling keyword mtlcolor
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

					// hanlding keyword sphere
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

					// default case throwing error
					throw -1;
			}

		};

		// checking required keywords have been read
		for(auto& i: headerMap){
			if(i.second == false && (i.first != "mtlcolor" && i.first != "sphere")) throw -1;
		}

		cout<<"Successfully read the input file."<<endl;
	}

	else throw -1;
	//close the input file
	inputFile.close();

	// return image parameters
	return id;

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

// Function to get normalized unit Vector
Vector normalize(Vector a){
	float magnitude = sqrt(a.dx*a.dx + a.dy*a.dy + a.dz*a.dz);
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

// Function to get negative of a given Vector
Vector negateVector(Vector a){
	Vector result = {-a.dx, -a.dy ,-a.dz};
	return result;
}

// Function to get cross products of 2 Vectors
Vector crossProduct(Vector a, Vector b){\
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

// Utility function to print Vector
void printVector(Vector a){
	cout<<a.dx<<","<<a.dy<<","<<a.dz<<","<<endl;
	return;
}

// Utility function to print a RayType
void printRay(RayType a){
	cout<<a.x<<","<<a.y<<","<<a.z<<","<<a.dx<<","<<a.dy<<","<<a.dz<<","<<endl;
	return;
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

// Function to get all the rays passing through the viewing window
vector<vector<RayType>> getRays(ImageParameters id){
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

// Function to check if a shpere and a ray interset
float findSphereIntersectionDistance(RayType ray, SphereType sphere){
	
	// find A, B, C of the quadtratic equation
	float A = 1.0;
	float B = 2.0 * ( ray.dx * (ray.x - sphere.cx) +  ray.dy * (ray.y - sphere.cy) + ray.dz * (ray.z - sphere.cz));
	float C = (ray.x - sphere.cx)*(ray.x - sphere.cx) + (ray.y - sphere.cy)*(ray.y - sphere.cy) + (ray.z - sphere.cz)*(ray.z - sphere.cz) - sphere.r*sphere.r;

	// check if solution exists
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

// Function to return the color of the intersection point
ColorType shadeRay(ImageParameters id, int objectId){
	return id.spheres[objectId].mtr.c;
}

// Function to trace ray and find intersections with given image parameters
ColorType traceRay(RayType ray, ImageParameters id){
	int objectId = -1;
	float minDistance = FLT_MAX;
	
	// iterate over all image objects
	for(int i=0;i<id.spheres.size();i++){

		// find if the object intersects
		float dist = findSphereIntersectionDistance(ray, id.spheres[i]);
		if(dist == FLT_MAX) continue;
		
		// check if the intersection point is closer than previous intersection point
		else if(minDistance > dist){
			minDistance = dist;
			objectId = i;
		}
	}

	// return the color if there is an intersection point else return background color
	if(minDistance != FLT_MAX) return shadeRay(id,objectId);
	else return id.bkgcolor;
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