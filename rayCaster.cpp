#include "rayCaster.h"
using namespace std;

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
Vector getRayPoint(RayType& ray, float t){
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
float findSphereIntersectionDistance(RayType& ray, SphereType sphere){
	
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
	area =1.0/area;

	float beta = getTriangleArea(t.v1, p, t.v3) * area;
	float gamma = getTriangleArea(t.v1, t.v2, p) * area;

	float alpha = getTriangleArea(p, t.v2, t.v3) * area;

	return {alpha, beta, gamma};
}

pair<float, vector<float>> findTriangleIntersectionDistance(RayType& ray, Triangle t){
	Vector e1 = getVector(t.v1, t.v2);
	Vector e2 = getVector(t.v1, t.v3);
	Vector n = crossProduct(e1,e2);

	float plainConstant = getPlainConstant(n, t.v1);

	float denominator = n.dx*ray.dx + n.dy*ray.dy + n.dz*ray.dz;

	if(denominator == 0.0)return {FLT_MAX, {}};

	float numerator = -(n.dx*ray.x + n.dy*ray.y + n.dz*ray.z + plainConstant);
	float distance = numerator/denominator; 

	if(distance<0.0) return {FLT_MAX, {}};
	
	Point p = {ray.x + distance*ray.dx, ray.y + distance*ray.dy, ray.z + distance*ray.dz};
	// printPoint(p);

	vector<float> coordinates = getBaricenterCoordiantes(t, p);

	if(coordinates[0]>1.0 || coordinates[1]>1.0 || coordinates[2]>1.0 || fabs(coordinates[0] + coordinates[1]+ coordinates[2] - 1.0) > EPI) return {FLT_MAX, {}};

	return {distance, coordinates};

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

RayType getReflectiveRay(RayType incidence, Vector normal, Vector pointOfIntersection){
	Vector newIncidence = {-incidence.dx, -incidence.dy, -incidence.dz};
	newIncidence = normalize(newIncidence);
	Vector r = multiplyScalar(normal, 2*(dotProduct(normal, newIncidence)));
	r = add(r, negateVector(newIncidence));
	RayType reflectiveRay = makeDirRay(pointOfIntersection, r);
	return reflectiveRay;
}

RayType getRefractiveRay(RayType incidence, Vector normal, Vector pointOfIntersection, float etaIncidence, float etaRefraction){
	Vector newIncidence = {-incidence.dx, -incidence.dy, -incidence.dz};
	newIncidence = normalize(newIncidence);
	normal = normalize(normal);
	float mul = sqrt(1.0 - (pow(etaIncidence/etaRefraction, 2)*(1.0 - pow(dotProduct(normal, newIncidence), 2.0))));
	Vector a = multiplyScalar(normal, mul);
	a = negateVector(a);

	Vector b = multiplyScalar(normal, dotProduct(normal, newIncidence));
	b = add(b, negateVector(newIncidence));
	b = multiplyScalar(b, etaIncidence/etaRefraction);

	Vector t = add(a,b);
	RayType refractiveRay = makeDirRay(pointOfIntersection, t);
	pointOfIntersection = getRayPoint(refractiveRay, EPI);
	refractiveRay = makeDirRay(pointOfIntersection, t);
	return refractiveRay;
}

float getFr(RayType incidence, Vector normal, float f0){
	Vector newIncidence = {-incidence.dx, -incidence.dy, -incidence.dz};
	newIncidence = normalize(newIncidence);
	normal = normalize(normal);
	float fr = f0 + (1.0-f0)*pow(1.0 - dotProduct(normal, newIncidence), 5.0);
	return fr;
}

// Function to return the color of the intersection point
ColorType shadeRay(ImageParameters& id, int objectId, int objectType, Vector pointOfIntersection, RayType& ray, int depth, stack<pair<int, float>> etaStack){

	// initialize color variables
	ColorType oA, oD = {0.0, 0.0, 0.0}, oS = {0.0, 0.0, 0.0}, sigma={0.0, 0.0, 0.0};
	Vector normal;
	Material objectMat;
	bool inside = false;

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
		vector<float> coordinates = ray.coordinates;
		if(object.normalFlag != true) normal = crossProduct(e1,e2);
		else{
			normal = add(add(multiplyScalar(object.vn1, coordinates[0]), multiplyScalar(object.vn2, coordinates[1])),
							multiplyScalar(object.vn3, coordinates[2]));
		}

		objectMat = object.mtr;

		if(object.textureFlag == true){
			// debug("Enter");
			float v = coordinates[0]*object.vt1.x + coordinates[1]*object.vt2.x + coordinates[2]*object.vt3.x;
			float u = coordinates[0]*object.vt1.y + coordinates[1]*object.vt2.y + coordinates[2]*object.vt3.y;

			int i = min(object.tex.height-1, (int)(v*(float)(object.tex.height-1)));
			int j = min(object.tex.width-1, (int)(u*(float)(object.tex.width-1)));

			// debug(v);
			// debug(u);
			// debug(i);
			// debug(j);
			objectMat.materialColor = object.tex.textureList[i][j];
			// debug("Exit");

		}
	}


	// ger normal vector from the point of intersection
	normal = normalize(normal);

	// get eye position vector
	Vector eyePosition = {id.eye.x, id.eye.y, id.eye.z};
	Vector V = add(eyePosition, negateVector(pointOfIntersection));


	ColorType res = oA;

	float fr = getFr(ray, normal, objectMat.f0);	

	RayType refractiveRay = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX};
	bool internalReflection = false;
	if(objectType == 0){

		pair<int, float> etaTop = {-1,1.0};
		float criticalAngle = 90.0;
		if(!etaStack.empty()) etaTop = etaStack.top(); 
		if(etaTop.first == objectId){
			normal = negateVector(normal);
			etaStack.pop();
			if(!etaStack.empty()) etaTop = etaStack.top(); 
			else etaTop = {-1,1.0};
			// debug(objectMat.eta);
			// debug(etaTop.second);
			fr = getFr(ray, normal, objectMat.f0);

			Vector incidence = {-ray.dx, -ray.dy, -ray.dz};
			incidence = normalize(incidence);
			float incidenceAngle = acos(dotProduct(normal, incidence))* 180.0 / PI;
			refractiveRay = getRefractiveRay(ray, normal, pointOfIntersection, objectMat.eta, etaTop.second);
			if(objectMat.eta > etaTop.second) criticalAngle = asin(etaTop.second / objectMat.eta)* 180.0 / PI;

			if(incidenceAngle >= criticalAngle && incidenceAngle <= 90.0) internalReflection = true;
			else etaStack.pop();


		} else {
			refractiveRay = getRefractiveRay(ray, normal, pointOfIntersection, etaTop.second, objectMat.eta);
			etaStack.push({objectId, objectMat.eta});
		}
	} else if(objectType == 1){
		float plainConstant = getPlainConstant(normal, id.triangles[objectId].v1);
		float rayDistance = normal.dx*ray.x + normal.dy*ray.y + normal.dz*ray.z + plainConstant;
		Vector refractiveNormal = normal;
		if(rayDistance < 0.0) refractiveNormal = negateVector(refractiveNormal);
		pair<int, float> etaTop = {-1,1.0};
		if(!etaStack.empty()) etaTop = etaStack.top();
		refractiveRay = getRefractiveRay(ray, refractiveNormal, pointOfIntersection, etaTop.second, objectMat.eta);
		refractiveRay = getRefractiveRay(refractiveRay, negateVector(refractiveNormal), pointOfIntersection, objectMat.eta, etaTop.second); 	
		// internalReflection = true;
	}


	if(depth +1 < DEPTHTHRESHOLD){
		
		if(refractiveRay.x != FLT_MAX && !internalReflection){
			ColorType refractiveColor = traceRay(refractiveRay, id, depth+1, etaStack, true);
			refractiveColor = multiplyScalar(refractiveColor, (1.0 - fr)*(1.0-objectMat.alpha));
			res = add(res, refractiveColor);
		}

		RayType reflectiveRay = getReflectiveRay(ray, normal, pointOfIntersection);
		ColorType reflectiveColor = traceRay(reflectiveRay, id, depth+1, etaStack, false);
		if(reflectiveColor.R >= 0.0 && reflectiveColor.G >=0.0 && reflectiveColor.B >= 0.0){
			reflectiveColor = multiplyScalar(reflectiveColor, fr);
			res = add(res, reflectiveColor);
		}
		
	}
	
	// traversing all the light sources
	for(auto& lightSource: id.lights){

		float shadowFlag=0.0;
		
		// getting the Light source vector
		Vector lightSourceVector = {lightSource.x, lightSource.y, lightSource.z};
		lightSourceVector = negateVector(lightSourceVector);


		// forming multiple shadow rays for making soft shadows
		for(int x = 0; x < SHADOWTESTTIMES; x++){
			Material shadownMat;

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
				
				if(i == objectId && objectType==0) continue;

				float dist = findSphereIntersectionDistance(shadowRay, id.spheres[i]);
				if(dist == FLT_MAX) continue;

				else if(minDistance > dist && dist > EPI){
					minDistance = dist;
					shadownMat = id.spheres[i].mtr;
				}
			}

					// iterate over all image triangle
			for(int i=0;i<id.triangles.size();i++){

				if(i == objectId && objectType == 1) continue;

				// find if the object intersects
				pair<float, vector<float>> dist = findTriangleIntersectionDistance(shadowRay, id.triangles[i]);
				if(dist.first == FLT_MAX) continue;
				
				// check if the intersection point is closer than previous intersection point
				else if(minDistance > dist.first && dist.first > EPI){
					minDistance = dist.first;
					shadownMat = id.triangles[i].mtr;
				}
			}

			//  calculate shadow flag
			if(lightSource.w == 0.0 && minDistance!=FLT_MAX) shadowFlag += (1.0 - shadownMat.alpha);
			else if(lightSource.w==1.0 && minDistance!=FLT_MAX){
				if(minDistance < getMagnitude( add(pointOfIntersection, lightSourceVector))) shadowFlag += (1.0 - shadownMat.alpha);
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
		nDotH = max((float)0.0, nDotH);
		nDotH = pow(nDotH, objectMat.n);

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

		// shadowFlag = 1.0;
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
ColorType traceRay(RayType& ray, ImageParameters& id, int depth, stack<pair<int, float>> etaStack, bool refractiveRayFlag){
	int objectId = -1;
	int objectType = -1;
	float minDistance = FLT_MAX;
	
	// iterate over all image spheres
	for(int i=0;i<id.spheres.size();i++){

		// find if the object intersects
		float dist = findSphereIntersectionDistance(ray, id.spheres[i]);
		if(dist == FLT_MAX) continue;
		
		// check if the intersection point is closer than previous intersection point
		else if(minDistance > dist && dist > EPI){
			minDistance = dist;
			objectId = i;
			objectType = 0;
		}
	}

	// iterate over all image triangle
	for(int i=0;i<id.triangles.size();i++){

		// find if the object intersects
		pair<float, vector<float>> dist = findTriangleIntersectionDistance(ray, id.triangles[i]);
		if(dist.first == FLT_MAX) continue;
		
		// check if the intersection point is closer than previous intersection point
		else if(minDistance > dist.first && dist.first > EPI){
			minDistance = dist.first;
			objectId = i;
			objectType = 1;
			ray.coordinates = dist.second;
		}
	}
	// return the color if there is an intersection point else return background color
	// if(refractiveRayFlag) return {0.0, 0.0, 0.0};
	if(minDistance != FLT_MAX){
		
		ColorType c = shadeRay(id,objectId, objectType, getRayPoint(ray, minDistance), ray, depth, etaStack);
		return c;
	}
	if(depth == 0 || refractiveRayFlag) return id.bkgcolor;
	else return {-1.0, -1.0, -1.0};
	// else return {-1.0, -1.0, -1.0};
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

		Vector eyePosition = {id.eye.x, id.eye.y, id.eye.z};
		RayType ray = makeDirRay(eyePosition, {3,0,-4});

		// traceRay({0.0, 0.0, 5.0, -0.2, -0.2, -5.0}, id);
		// traceRay({0.0, 0.0, 5.0, 0.2, -0.2, -5.0}, id);
		// traceRay({0.0, 0.0, 5.0, -0.2, 0.2, -5.0}, id);

		for(int i=0;i<id.dim.height;i++){
			for(int j=0;j<id.dim.width;j++){
				stack<pair<int, float>> etaStack;
				etaStack.push({-1,1.0});
				image[i][j] = traceRay(rays[i][j], id, 0, etaStack, false);
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