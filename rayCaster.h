#include <iostream>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <fstream>
#include "models.h"
#include "utils.h"
#include "file_utils.h"

#ifndef RAYCASTER_H
#define RAYCASTER_H

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

#endif