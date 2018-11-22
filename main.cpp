#include <utility>
#include <cmath>
#include <vector>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 200;
const int height = 200;
const float EPSILON = 0.001;

void line(Vec2i vec1, Vec2i vec2, TGAImage &image, TGAColor color)
{
    bool steep = false;
    // if the line is steep, transpose it
    if (std::abs(vec1.x - vec2.x) < std::abs(vec1.y - vec2.y))
    {
        std::swap(vec1.x, vec1.y);
        std::swap(vec2.x, vec2.y);
        steep = true;
    }
    // draw image from left to right
    if (vec1.x > vec2.x)
    {
        std::swap(vec1.x, vec2.x);
        std::swap(vec1.y, vec2.y);
    }
    
    int dx = vec2.x - vec1.x;
    int dy = vec2.y - vec1.y;
    float derror = std::abs(dy / float(dx));
    float error = 0;  // distance from best straight line to current pixel
    int y = vec1.y;                        
       
    for (int x = vec1.x; x <= vec2.x; x++)
    {
        if (steep)
        {
            image.set(y, x, color);  // de-transpose image
        }
        else
        {
            image.set(x, y, color);
        }
        
        error += derror;
        // if the distance from the best straight line to (vec2.x, vec2.y) is greater than a pixel
        if (error > .5)
        {
            y += (vec2.y > vec1.y ? 1 : -1);
            error -= 1.0;
        }
        
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
     bool steep = false;
 
     // if the line is steep, transpose it
     if (std::abs(x0 - x1) < std::abs(y0 - y1))
     {
         std::swap(x0, y0);
         std::swap(x1, y1);
         steep = true;
     }
 
     // draw image from left to right
     if (x0 > x1)
     {
         std::swap(x0, x1);
         std::swap(y0, y1);
     }
     
     int dx = x1 - x0;
     int dy = y1 - y0;
     float derror = std::abs(dy / float(dx));
     float error = 0;  // distance from best straight line to current pixel
     int y = y0;                               
     for (int x = x0; x <= x1; x++)
     {
         if (steep)
         {
             image.set(y, x, color);  // de-transpose image
         }
         else
         {
             image.set(x, y, color);
         }
         
         error += derror;
         // if the distance from the best straight line to (x1, y1) is greater than a pixel
         if (error > .5)
         {
             y += (y1 > y0 ? 1 : -1);
             error -= 1.0;
         }
    }
}

// computes the barycentric coordinates of a point
Vec3f barycentric(Vec2i *pts, Vec2i P) { 

    // take the cross product of these two vectors
    Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^
                Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
    
    // triangle is not formed correctly
    if (std::abs(u.z)<1) 
    {
        return Vec3f(-1,1,1);
    }

    return Vec3f(1.f -(u.x + u.y) / u.z, u.y / u.z, u.x / u.z); 
} 
 
void triangle(Vec2i *pts, TGAImage &image, TGAColor color) 
{ 
    Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width() - 1, image.get_height() - 1); 

    // goes through triangle vertices to determine dimensions of bounding box
    for (int i = 0; i < 3; i++) 
    { 
        for (int j = 0; j < 2; j++) 
        { 
            bboxmin.raw[j] = std::max(0, std::min(bboxmin.raw[j], pts[i].raw[j])); 
            bboxmax.raw[j] = std::min(clamp.raw[j], std::max(bboxmax.raw[j], pts[i].raw[j])); 
        } 
    } 

    Vec2i P; 
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) 
    { 
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) 
        { 
            Vec3f bc_screen  = barycentric(pts, P); 

            // negative barycentric coordinates mean that the pixel is not in bounding box
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) 
            {
                continue; 
            }

            image.set(P.x, P.y, color); 
        } 
    } 
} 
 
int main(int argc, char** argv) 
{ 
    if (argc == 2)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f lightDir(0,0,-1); // the direction the light is coming from

    for (int i = 0; i < model->nfaces(); i++) 
    { 
        std::vector<int> face = model->face(i); 
        Vec3f worldCoords[3];  // regular coordinates
        Vec2i screenCoords[3]; // coordinates scaled to screen dimensions
    
        for (int j = 0; j < 3; j++) 
        { 
            Vec3f v = model->vert(face[j]);
            screenCoords[j] = Vec2i((v.x + 1.0) * width / 2.0, (v.y + 1.0) * height / 2.0);
            worldCoords[j] = v;
        } 

        // gets the normal vector of a triangle face
        Vec3f n = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
        n.normalize();

        // intensity is the dot product of the light direction and normal vector of faces
        float intensity = n * lightDir;

        if (intensity > 0)
        {
            triangle(screenCoords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255)); 
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
