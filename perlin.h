#ifndef PERLIN_H
#define PERLIN_H

#include "glwidget.h"
#include <vector>

class PerlinNoise {
    int rows, columns;
    std::vector<unsigned char> gradients;
    static const QVector2D quad[];

    //Linear interpolation
    static float mix(float a, float b, float r);

    //fade function
    static float fade(float t);

    void inititalizeGradrient();

public:
    //Perlin noise at point x,y
    float perlinSample(float x, float y) const;

    float octavePerlin(float x, float y, int octaves, double persistence) const;

    PerlinNoise() {}
    //max size
    PerlinNoise(int rows, int columns);
    PerlinNoise(int rows, int columns, unsigned int seed);

    float *genPerlinTexture(int w, int h, float offset, float incr, int octaves, float persistence) const;
};

#endif
