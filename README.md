
# Computer Graphics Course Project (2023)
This is a project for the PG1 course at VSB-TUO. The project is a ray tracer and a path tracer, implemented in C++.
Base-structure of the project and models were provided by the teacher to build upon.
List of implemented features can be found below.  

For anyone in the course right now, I wish you good luck, it might be a bit challenging, but it's doable.
The key is to ask question, debug debug debug and have fun with it.

![](results\spaceship.png)

## Ray Tracing
- most of these can be found in the `RayTracer` class, in the `raytracer.cpp` file
1. [x]  Pin hole camera
1. [x]  Normal shader
    - use normal vector as the color of the pixel
2. [x]  Lambert shader
1. [x]  Phong/Whitted shader
1. [x]  Hard shadows
    - check if object is visible from the light source by casting a ray from the intersection point to the light source
    - if the ray intersects with another object, the point is in shadow
1. [x] Textures with bilinear interpolation
    - Texture class with loaded
1. [x]  Reflections
    - mirror shader
    - recursive call to trace_ray with a new (secondary) ray
1. [x]  Refractions
    - glass shader
1. [x]  Environmental map (spherical map)
    - SphereMap class with loaded background image
1. [x]  Gamma correction
    - methods in the Color4f class in stucts.cpp
    - expand for Vec3f calculations, compress for the final color
1. [x]  Super sampling
    - for each pixel, cast multiple rays with random offset and average the colors
1. [x]  Depth of field
    - in the camera class
    - debug to calculate the focus distance (travel distance of the ray from the camera to the object)
1. [x]  BVH - Bounding volume hierarchy (construction and traversal) - time consuming
1. [ ]  SAH - not implemented

![](results\ray_mirror.png)

## Path Tracing
- most of these can be found in the `PathTracer` class, in the `pathtracer.cpp` file
1. [x]  Mirror BRDF
    - in the camera class
1. [x]  Lambert BRDF
1. [x]  Russian roulette
    - don't continue the path if the ray is "absorbed" (using random treshold)
1. [x]  Modified Phong BRDF (energy-conserved)
    - Lafortune and Willems’ Method
    - specualar sampling and cosine-lobe sampling
1. [ ]  Modified Phong BRDF (energy-normalized) - WIP
    - Arvo’s Method
    - Mallett’s and Yuksel’s Method
    - (issues with Furnace's test at 90 degrees)
1. [ ]  Next event estimation (direct light sampling) - WIP
    - sample the light source directly
    - area form sampling
1. [ ]  PBR with Cook-Torrance BRDF - not implemented - not implemented
1. [ ]  Image-based lighting (with and w/o importance sampling) - not implemented

![](results\pathtracer.png)