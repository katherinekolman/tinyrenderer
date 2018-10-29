#include "tgaimage.h"
#include <utility>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

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
	TGAImage image(100, 100, TGAImage::RGB);
	image.set(52, 41, red);
    line(20, 80, 70, 10, image, white);
	image.flip_vertically();  // origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

