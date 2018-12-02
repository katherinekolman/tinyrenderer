# tinyrenderer

This is my attempt at recreating ssloy's tinyrenderer (put link here).
I'll be documenting what I've learned here.


## Concepts Learned
* Lesson 1: Bresenham's Line Drawing Algorithm
  - approximates a pixel line from one point to another using the point-slope formula + error bounds on y
  - uses y-value error bound to determine which pixel to color  
  - reading and processing .obj files to make wireframe models

* Lesson 2: Triangle rasterization and back-face culling
  - how to draw a triangle to the screen:
     - old, simple way: divide a triangle in half, then draw and fill in each part separately
     - new, simple way: use barycentric coordinates to calculate whether a pixel in a set bounding box is within the triangle
       - if none of the coordinates are negative, then the pixel is within the triangle 
  - back-face culling: don't light the triangles that aren't visible
     - calculating the intensity of the light: dot product of light vector and triangle's normal vector  

* Lesson 3: Hidden faces removal (z-buffer)
  - painter's algorithm doesn't work for everything
  - for 2D: use y-buffer to render things
    - render the things closest to the camera

