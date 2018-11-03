/**
 * This is a parser for models in .obj format.
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

// parses a obj file for the vertices and faces
Model::Model(const char *filename) : verts_(), faces_()
{
    std::ifstream in;

    // opens obj file
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash; 
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash; // get rid of the "v " part

            // add the coordinates to a new vector
            Vec3f v;
            for (int i = 0; i < 3; i++)
            {
                iss >> v.raw[i];
            }

            // add new vertice to list of vertices
            verts_.push_back(v);
        } 
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            int itrash, idx;

            iss >> trash; // gets rid of the "f " part

            // gets the first number after each space, adds it to a new
            // vector, and adds the vector to the list of faces
            while (iss >> idx >> trash >> itrash >> trash >> itrash)
            {
                idx--;
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
    }
    
    std::cerr << "# v# " << verts_.size() << " f#" << faces_.size() << std::endl;
}

// destructor
Model::~Model() {}

// returns the number of vertices
int Model::nverts()
{
    return (int)verts_.size();
}

// returns the number of faces
int Model::nfaces()
{
    return (int)faces_.size();
}

// returns the 3 vertices that make up a face as a vector at index i
std::vector<int> Model::face(int i)
{
    return faces_[i];
}

// returns the vertice at index i
Vec3f Model::vert(int i)
{
    return verts_[i];
}







