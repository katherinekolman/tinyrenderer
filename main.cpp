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

void drawWireframe(Model *model, TGAImage &image)
{
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);

        // draws a line from a point to the other vertices in the face
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1) % 3]);
            int x0 = (v0.x + 1.0) * width / 2.0;
            int y0 = (v0.y + 1.0) * height / 2.0;
            int x1 = (v1.x + 1.0) * width / 2.0;
            int y1 = (v1.y + 1.0) * height / 2.0;
            line(x0, y0, x1, y1, image, white);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if (t0.y>t1.y) std::swap(t0, t1); 
    if (t0.y>t2.y) std::swap(t0, t2); 
    if (t1.y>t2.y) std::swap(t1, t2); 

    Vec2i a;
    if (t2.x != t0.x)
    {
        float newX = t0.x + ((t1.y - t0.y) * (t2.x - t0.x))/(t2.y - t0.y);
        a = Vec2i((int)(newX), t1.y);
    }

    line(t0, t1, image, green);
    line(a, t0, image, red);

    int x = t0.x;
    for (int y = t0.y; y <= a.y; y++)
    {
        int leftX = int(t0.x + ((y - t0.y) * (a.x - t0.x))/(a.y - t0.y));
        int rightX = int(t0.x + ((y - t0.y) * (t1.x - t0.x))/(t1.y - t0.y));
        line(leftX, y, rightX, y, image, white);
    }
    
    for (int y = t2.y; y > t1.y; y--) {
        int leftX = int(t2.x + ((y - t2.y) * (a.x - t2.x))/(a.y - t2.y));
        int rightX = int(t2.x + ((y - t2.y) * (t1.x - t2.x))/(t1.y - t2.y));
        line(leftX, y, rightX, y, image, white);
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
    //drawWireframe(model, image);
 
    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
    triangle(t0[0], t0[1], t0[2], image, red); 
    triangle(t1[0], t1[1], t1[2], image, white); 
    triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // want origin at the bottom left corner
    image.write_tga_file("output.tga");
    delete model;
	return 0;
}

