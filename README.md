
# Raycaster Algorithm for Windows





## How to use?

- Run the cpp file (you need a c++ compiler)
- It'll generate a PPM file that you can either open on the web or photoshop.
- Edit the values in the main to change the output.

## Explanation

this is a raytracer algorithm that makes spheres with transmissive and reflective properties and output a PPM file.

### How a 3d view is translated into a 2d screen:

Ray tracing algorithms: the most straightforward way of simulating the physical phenomena that cause objects to be visible


We can visualize a picture as a cut made through a pyramid whose apex is located at the center of our eye and whose height is parallel to our line of sight.

It's a 2 step process:

- projecting the shapes of the three dimensional objects.
- outline created is projected on the image plane.

Photons strike an object and are either absorbed or reflected.

Let's assume our image plane = the eyes => in that case, a photon emitted will hit one of the pixels in the screen.

problem: photons go in every direction, meaning an impossibly large simulation.
can we aim it to only hit the screen? sure. But it wouldn't be optimal

say we do try the first method; we'd have to let the program decide how many photons to spray until the image is of satisfatory results

spraying a constant number of photons wouldn't fill the whole image. only parts of it because we dont have enough.

Conclusion: Light tracing, or forward tracing is technically possible to simulate light on computers but not really viable. 

### Solution

Backward tracing: or eye tracing, lets do the opposite and trace a ray from our eyes to the object to the light.

**That's the basic norm of CGI today**

Why wasn't ray tracing adopted back in the day instead of wireframing?

It was slow. HELLA slow.

Compared to z-buffer or rasterization algo, ray tracing is still slower, but computers have evolved to the point where its possible to render in minutes.

Raytracing is the defacto standard now for rendering in offline software. 


Let's look at reflection and refraction before we write our code.

If the ray hits an object that is not diffuse or opaque, we must do extra computational work.

Compute the reflection color, compute the refraction color, and then apply the Fresnel equation.

Reflection need the normal point of intersection and the light direction

Refraction however need those two and a third one: IOR (Index of Refraction)

- Refraction occurs every time there's a change of medium
- Fresnel equation mixes the two(Light cant be created or destoryed) => reflection mix value + refraction mix value = 1;
- Snell's Law: youll get it once u see the code.

Comments are everywhere, have fun !!
