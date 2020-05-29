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
	float alpha;
	float f0;
	float eta;
};

struct Texture{
	int width;
	int height;
	std::vector<std::vector<ColorType>> textureList;
};

// Strucf for a sphere
struct SphereType{
	float cx, cy, cz;
	float r;
	Material mtr;
	Texture tex;
	bool textureFlag;
};


// Struct for a RayType
struct RayType{
	float x, y, z;
	float dx, dy, dz;
	std::vector<float> coordinates;
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

struct LightSource{
	float x, y, z;
	float w;
	ColorType c;
	int attFlag;
	float c1, c2, c3;
};

struct DepthCue{
	ColorType c;
	float amax, amin;
	float dmax, dmin;
};

struct Triangle{
	Point v1, v2, v3;
	Point vt1, vt2, vt3;
	Vector vn1, vn2, vn3;

	bool normalFlag;
	bool textureFlag;

	Material mtr;
	Texture tex;
};

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
	DepthCue depthCue;
	bool depthFlag;
	std::vector<SphereType> spheres;
	std::vector<Point> vertices;
	std::vector<Point> verticeTextures;
	std::vector<Vector> verticeNormals;
	std::vector<Triangle> triangles;
	std::vector<LightSource> lights;
};

ColorType traceRay(RayType& ray, ImageParameters& id, int depth, std::stack<std::pair<int, float>> etaStack, bool refractiveRayFlag);
ColorType shadeRay(ImageParameters& id, int objectId, int objectType, Vector pointOfIntersection, RayType& ray, int depth, std::stack<std::pair<int, float>> etaStack);

// Constants

// Distance of viewing window from the eye
#define D 200

// Value of Pie
#define PI 3.14159265

#define JITTERDIST 0.0006

#define SHADOWTESTTIMES 5

#define EPI 0.00005

#define DEPTHTHRESHOLD 3