#include <utility>
#include <cmath>
#include <vector>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

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

    image.flip_vertically(); // want origin at the bottom left corner
    image.write_tga_file("output.tga");
    delete model;
	return 0;
}

