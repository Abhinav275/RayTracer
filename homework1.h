#include <bits/stdc++.h>

// Struct for RGB colors
struct ColorType{
	float R;
	float G;
	float B;
};

// Struct for Material
struct Material{
	ColorType materialColor, specColor;
	float ka, kd, ks;
	float n;
};

// Strucf for a sphere
struct SphereType{
	float cx, cy, cz;
	float r;
	Material mtr;
};


// Struct for a RayType
struct RayType{
	float x, y, z;
	float dx, dy, dz;
};

// Struct for iamge dimensions
struct ImageDimension{
	int width;
	int height;
};

// Struct for a point in cxz coordinate system
struct Point{
	float x, y, z;
};


// Struct for a vector
struct Vector{
	float dx, dy, dz;
};

// Struct for making a viewing window
struct ViewingWindow{
	Vector ul;
	Vector ur;
	Vector ll;
	Vector lr;
};

struct LightSouce{
	float x, y, z;
	int w;
	ColorType c;
}

// Struct to store all the image parameters
struct ImageParameters{
	Point eye;
	Vector viewdir;
	Vector up;
	Vector u;
	Vector v;
	Vector w;
	ImageDimension dim;
	float hfov;
	ViewingWindow vw;
	ColorType bkgcolor;
	std::vector<SphereType> spheres;
	std::vector<LightSouce> lights;
};

// Constants

// Distance of viewing window from the eye
#define D 5

// Value of Pie
#define PI 3.14159265

#define JITTERDIST 0.5

#define SHADOWTESTTIMES 50