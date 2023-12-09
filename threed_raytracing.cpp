
/// How a 3d view is translated into a 2d screen:
/// Ray tracing algorithms: the most straightforward way of simulating the physical phenomena that cause objects to be visible


// We can visualize a picture as a cut made through a pyramid whose apex is located at the center of our eye and whose height is parallel to our line of sight.
// It's a 2 step process:
	// projecting the shapes of the three dimensional objects.
	// outline created is projected on the image plane.

// Photons strike an object and are either absorbed or reflected.

// Let's assume our image plane = the eyes => in that case, a photon emitted will hit one of the pixels in the screen.
// problem: photons go in every direction, meaning an impossibly large simulation.
// can we aim it to only hit the screen? sure. But it wouldn't be optimal

// say we do try the first method; we'd have to let the program decide how many photons to spray until the image is of satisfatory results
// spraying a constant number of photons wouldn't fill the whole image. only parts of it because we dont have enough.
// Conclusion: Light tracing, or forward tracing is technically possible to simulate light on computers but not really viable. 

// Backward tracing: or eye tracing, lets do the opposite and trace a ray from our eyes to the object to the light.
// that's the basic norm of CGI today

// Why wasn't ray tracing adopted back in the day instead of wireframing?
// It was slow. HELLA slow.
// Compared to z-buffer or rasterization algo, ray tracing is still slower, but computers have evolved to the point where its possible to render in minutes.
// Raytracing is the defacto standard now for rendering in offline software. 


// Let's look at reflection and refraction before we write our code.
// If the ray hits an object that is not diffuse or opaque, we must do extra computational work.
// Compute the reflection color, compute the refraction color, and then apply the Fresnel equation.

// Reflection need the normal point of intersection and the light direction
// Refraction however need those two and a third one: IOR (Index of Refraction)
// Refraction occurs every time there's a change of medium
// Fresnel equation mixes the two
// (Light cant be created or destoryed) => reflection mix value + refraction mix value = 1;



/// Let's Begin



#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

template<typename T>

class Vec3
{
public:
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3<T> operator * (const T& f) const { return Vec3<T>(x * f, y * f, z * f); }
	Vec3<T> operator * (const Vec3<T>& v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3<T> operator - (const Vec3<T>& v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator + (const Vec3<T>& v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T>& operator += (const Vec3<T>& v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3<T>& operator *= (const Vec3<T>& v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream& operator << (std::ostream& os, const Vec3<T>& v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};


typedef Vec3<float> Vec3f;


class Sphere {
public: Vec3f center;
	  float radius, radius2; /// precalculate for efficiency
	  Vec3f surfaceColor, emissionColor; /// sphere color and surface emission color
	  float transparency, reflection; /// 0 to 1

	  Sphere(
		  const Vec3f& c,
		  const float& r,
		  const Vec3f& sc,
		  const float& refl = 0,
		  const float& transp = 0,
		  const Vec3f& ec = 0) :
		  center(c), radius(r), radius2(r* r), surfaceColor(sc), emissionColor(ec), transparency(transp), reflection(refl)
	  {
		  // Empty 
	  }

	  /// Let's go line by line in real life terms

	  bool intersect(const Vec3f& rayorig, const Vec3f& raydir, float& t0, float& t1) const {

		  Vec3f l = center - rayorig;			/// distance from eye to sphere origin
		  float tca = l.dot(raydir);			/// converting vector distance to a float number
		  if (tca < 0) return false;			/// it doesn't intersect if the ray is pointing away from it
		  float d2 = l.dot(l) - tca * tca;		/// squares distance between ray origin and center of sphere
		  if (d2 > radius2) return false;		/// ray is inside sphere, so no intersection
		  float thc = sqrt(radius2 - d2);		/// This line calculates the distance from the time of closest approach to the intersection point(s) on the sphere's surface. This is achieved by taking the square root of the difference between the squared radius and the squared distance.
		  t0 = tca - thc;						/// First intersection point
		  t1 = tca + thc;						/// Second intersection point

		  return true;							/// ray does intersect with a sphere
	  }
};



#define MAX_RAY_DEPTH 5

float mix(const float& a, const float& b, const float& mix) {
	return b * mix + a * (1 - mix);
}


Vec3f trace(
	const Vec3f &rayorig,
	const Vec3f &raydir,
	const std::vector<Sphere> &spheres,
	const int &depth) 

{

	float tnear = INFINITY;
	const Sphere* sphere = NULL;

	for (unsigned i = 0; i < spheres.size(); i++) {
		float t0 = INFINITY, t1 = INFINITY;

		if (spheres[i].intersect(rayorig, raydir, t0, t1)) {
			if (t0 < 0) t0 = t1;
			if (t0 < tnear) {
				tnear = t0;
				sphere = &spheres[i];
			}
		}
	}

	
	if (!sphere) return Vec3f(2);															/// no intersection => black or bg
	Vec3f surfaceColor = 0;													
	Vec3f phit = rayorig + raydir * tnear;													/// Point of Intersection
	Vec3f nhit = phit - sphere->center;														/// Normal at Intersection
	nhit.normalize();																		/// Normalize it

	float bias = 1e-4;																		/// avoid self-intersection issues.
	bool inside = false;

	if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true;									/// you're inside the sphere


	if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {	/// Welcome to reflective, transmissive materials
		float facingratio = -raydir.dot(nhit);												/// ?
		float fresnelEffect = mix(pow(1 - facingratio, 3), 1, 0.1);							/// ammount of light reflected
		/// No need to normalize since everything was already normalized

		Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
		refldir.normalize();
		Vec3f reflection = trace(phit + nhit * bias, refldir, spheres, depth + 1);
		Vec3f refraction = 0;

		if (sphere->transparency) {
			float ior = 1.1, eta = (inside) ? ior : 1 / ior;								/// Favorite line of code: Checks if you're inside or not to determine the index of refraction
			float cosi = nhit.dot(raydir);
			float k = 1 - eta * eta * (1 - cosi * cosi);									/// Discriminant of Snell's Law
			Vec3f refrdir = raydir * eta + nhit * (eta * cosi - sqrt(k));					/// refracted ray direction based on Snell's law.
			refrdir.normalize();
			refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);
		}
		
		// the result is a mix of reflection and refraction (if the sphere is transparent)

		surfaceColor = (reflection * fresnelEffect + refraction * (1 - fresnelEffect) * sphere->transparency) * sphere->surfaceColor;
	}
		else {
			/// Opaque or Diffuse Object (easy (Actually Not))
			for (unsigned i = 0; i < spheres.size(); ++i) {
				if (spheres[i].emissionColor.x > 0) {
					Vec3f transmission = 1;
					Vec3f lightDirection = spheres[i].center - phit;
					lightDirection.normalize();

					for (unsigned j = 0; j < spheres.size(); ++j) {
						if (i != j) {
							float t0, t1;
							if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) {
								transmission = 0;
								break;
							}
						}
					}

					/// the contribution of a light source to the color of a specific point on a sphere.
					surfaceColor += sphere->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}

		return surfaceColor + sphere->emissionColor;
	}


/// Up to this point, if I click debug and it actually works...


void render(const std::vector<Sphere>& spheres) {
	
	unsigned width = 640, height = 480;
	Vec3f *image = new Vec3f[width * height], *pixel = image;
	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30, aspectRatio = width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180.);
	// Trace rays
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x, ++pixel) {
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec3f raydir(xx, yy, -1);
			raydir.normalize();
			*pixel = trace(Vec3f(0), raydir, spheres, 0);
		}
	}

	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::ofstream ofs("./prettyTitle.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
			(unsigned char)(std::min(float(1), image[i].y) * 255) <<
			(unsigned char)(std::min(float(1), image[i].z) * 255);
	}
	ofs.close();
	delete[] image;
}



int main(int argc, char** argv) {

	std::vector<Sphere> spheres;
	// Position, radius, surface color, reflectivity, transparency, emission color
	spheres.push_back(Sphere(Vec3f( 0.0, -10004, -20),	1000, Vec3f(0.51, 0.00, 0.20), 0, 0.0));
    spheres.push_back(Sphere(Vec3f( 0.0,      0, -20),   2, Vec3f(0.40, 0.32, 0.96), 1, 0.5));
    spheres.push_back(Sphere(Vec3f( 5.0,     -1, -15),	  1, Vec3f(0.50, 0.96, 0.16), 1, 0.0));
    spheres.push_back(Sphere(Vec3f( 5.0,      0, -25),   4, Vec3f(0.55, 0.57, 0.57), 1, 0.0));
    spheres.push_back(Sphere(Vec3f(-5.5,      0, -15),     1, Vec3f(0.50, 0.00, 0.50), 1, 0.0));

	// Light
	spheres.push_back(Sphere(Vec3f(0.0f, 20.0f, -30.0f), 3.0f, Vec3f(0.00f, 0.00f, 0.00f), 0.0f, 0.0f, Vec3f(3.0f, 3.0f, 3.0f)));
	render(spheres);
	return 0;
}
