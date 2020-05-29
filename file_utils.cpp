#include "file_utils.h"

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

// function to read texture files
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

		float mColor = 1.0/(float)maxColor;

		while(colorListIndex<limit && getline(inputFile, line)){
			vector<string> tokens = getTokens(line, ' ');
			for(auto color: tokens){
				switch(colorIndex%3){
					case 0: {
						result[colorListIndex].R = (float)stoi(color)*mColor;
						colorIndex++;
						break;
					}
					case 1: {
						result[colorListIndex].G = (float)stoi(color)*mColor;
						colorIndex++;
						break; 
					}
					case 2: {
						result[colorListIndex].B = (float)stoi(color)*mColor;
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
					if(tokens.size()!=13 || !checkFloat(tokens[1]) || !checkFloat(tokens[2]) || !checkFloat(tokens[3]) ||
						!checkFloat(tokens[4]) || !checkFloat(tokens[5]) || !checkFloat(tokens[6]) ||
						!checkFloat(tokens[7]) || !checkFloat(tokens[8]) || !checkFloat(tokens[9]) ||
						!checkFloat(tokens[10]) || !checkFloat(tokens[11]) || !checkFloat(tokens[12])) throw -1;
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

 					float alpha = stof(tokens[11]);
 					
 					float eta = stof(tokens[12]);
 					float f0 = ((eta - 1)/(eta+1))*((eta - 1)/(eta+1));

					if(r<0.0 || r>1.0 || g<0.0 || g>1.0 || b<0.0 || b>1.0) throw -1;
					if(r1<0.0 || r1>1.0 || g1<0.0 || g1>1.0 || b1<0.0 || b1>1.0) throw -1;
					if(alpha<0.0 || alpha>1.0) throw -1;
					if(eta<1.0) throw -1;
					headerMap["mtlcolor"] = true;
					ColorType mtlcolor = {(float)r, (float)g, (float)b};
					ColorType speccolor = {(float)r1, (float)g1, (float)b1};
					mtr = {mtlcolor, speccolor, ka, kd, ks, n, alpha, f0, eta};
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
					f.textureFlag = false;
					f.normalFlag = false;

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
					// cout<<headers[tokens[0]]<<endl;
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
