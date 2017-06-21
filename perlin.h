#ifndef PERLIN_H
#define PERLIN_H

class PerlinNoise {
    int rows, columns;
    int w,h;

    //Linear interpolation
    float mix(float a, float b, float r);

    //Perlin noise at point x,y
    float perlinSample(float x, float y);

    //Dot product of vectors vec1 and vec2
    float dot(float vec1[2], float vec2[2]);

public:
    PerlinNoise(int rows, int columns, int width, int height);
};

#endif
