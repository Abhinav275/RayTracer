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
	int cx, cy, cz;
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
	int x, y, z;
};

struct Vector{
	int dx, dy, dz;
};

struct ImageDescription{
	Point eye;
	Vector viewdir;
	Vector up;
	ImageDimension dim;
	int hfov;
	ColorType bkgcolor;
	std::vector<SphereType> spheres;
};
