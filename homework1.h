#include <bits/stdc++.h>
struct ColorType{
	float R;
	float G;
	float B;
};

struct Material{
	ColorType c;
};

struct SphereType{
	float cx, cy, cz;
	float r;
	Material mtr;
};

struct RayType{
	float x, y, z;
	float dx, dy, dz;
};

struct ImageDimension{
	int width;
	int height;
};

struct Point{
	float x, y, z;
};

struct Vector{
	float dx, dy, dz;
};

struct ViewingWindow{
	Vector ul;
	Vector ur;
	Vector ll;
	Vector lr;
};

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
};

#define D 5
#define PI 3.14159265