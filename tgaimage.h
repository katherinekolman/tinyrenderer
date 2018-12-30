#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1)
struct TGA_Header 
{
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};
#pragma pack(pop)



struct TGAColor 
{
    unsigned char rgba[4];
    unsigned char bytespp;

    TGAColor() : rgba(), bytespp(1) 
    {
        for (int i = 0; i < 4; i++)
        {
            rgba[i] = 0;
        }
    }

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A=255): rgba(), bytespp(4)
    {
        rgba[0] = R;
        rgba[1] = G;
        rgba[2] = B;
        rgba[3] = A;
    }
        
	TGAColor(unsigned char v) : rgba(), bytespp(1) 
    {
        for (int i = 0; i < 4; i++)
        {
            rgba[i] = 0;
        }
        rgba[0] = v;
	}

	TGAColor(const unsigned char *p, unsigned char bpp) : rgba(), bytespp(bpp) 
    {
		for (int i = 0; i < (int)bpp; i++) 
        {
			rgba[i] = p[i];
		}

        for (int i = bpp; i < 4; i++) 
        {
            rgba[i] = 0;
        }
	}

    unsigned char& operator[](const int i) { return rgba[i]; }

    TGAColor operator *(float intensity) const
    {
        TGAColor res = *this;
        intensity = (intensity > 1.f ? 1.f : (intensity > 0.f ? 0.f : intensity));
        for (int i = 0; i < 4l; i++)
        {
            res.rgba[i] = rgba[i] * intensity;
        }
        return res;
    }

};


class TGAImage 
{
protected:
	unsigned char* data;
	int width;
	int height;
	int bytespp;

	bool load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out);
public:
	enum Format 
    {
		GRAYSCALE=1, RGB=3, RGBA=4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);
	bool read_tga_file(const char *filename);
	bool write_tga_file(const char *filename, bool rle=true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y);
	bool set(int x, int y, TGAColor &c);
    bool set(int x, int y, const TGAColor &c);
	~TGAImage();
	TGAImage & operator =(const TGAImage &img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char *buffer();
	void clear();
};

#endif //__IMAGE_H__
