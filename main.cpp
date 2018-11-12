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
const int width  = 200;
const int height = 200;

// computes the barycentric coordinates of a point
Vec3f barycentric(Vec2i *pts, Vec2i P) { 
    // take the cross product of these two vectors
    Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^
                Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
    
    // triangle is degenerate
    if (std::abs(u.z)<1) return Vec3f(-1,1,1);

    return Vec3f(1.f -(u.x + u.y) / u.z, u.y / u.z, u.x / u.z); 
} 
 
void triangle(Vec2i *pts, TGAImage &image, TGAColor color) { 
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 

    // goes through triangle vertices to determine dimensions of bounding box
    for (int i = 0; i < 3; i++) { 
        for (int j = 0; j < 2; j++) { 
            bboxmin.raw[j] = std::max(0,            std::min(bboxmin.raw[j], pts[i].raw[j])); 
            bboxmax.raw[j] = std::min(clamp.raw[j], std::max(bboxmax.raw[j], pts[i].raw[j])); 
        } 
    } 

    Vec2i P; 
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) { 
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts, P); 

            // negative barycentric coordinates mean that the pixel is not in bounding box
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 

            image.set(P.x, P.y, color); 
        } 
    } 
} 
 
int main(int argc, char** argv) { 
    TGAImage frame(200, 200, TGAImage::RGB); 

    Vec2i pts[3] = {Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160)}; 
    triangle(pts, frame, red); 

    frame.flip_vertically(); // to place the origin in the bottom left corner of the image 
    frame.write_tga_file("framebuffer.tga");
    return 0; 
}
