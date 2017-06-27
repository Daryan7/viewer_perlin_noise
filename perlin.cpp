#include "perlin.h"
#include <ctime>
#include <cstdlib>
#include <cmath>
#include "goodperlin.h"
using namespace std;

PerlinNoise::PerlinNoise(int rows, int columns) {
    this->rows = rows;
    this->columns = columns;
    srand(time(NULL));

    inititalizeGradrient();
}

PerlinNoise::PerlinNoise(int rows, int columns, unsigned int seed) {
    this->rows = rows;
    this->columns = columns;
    srand(seed);

    inititalizeGradrient();
}

const QVector2D PerlinNoise::quad[] = {
    QVector2D(1,1),
    QVector2D(-1,1),
    QVector2D(1,-1),
    QVector2D(-1,-1),
    QVector2D(sqrt(2), 0),
    QVector2D(0, sqrt(2)),
    QVector2D(-sqrt(2), 0),
    QVector2D(0, -sqrt(2))
};

void PerlinNoise::inititalizeGradrient() {
    gradients = vector<unsigned char>(rows*columns);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            gradients[i*columns + j] = rand()%8;
        }
    }
}

//float mn, mx;
//bool first = true;

float PerlinNoise::perlinSample(float x, float y) const {
    int x0 = x;
    int y0 = y;
    int x1 = x0+1;
    int y1 = y0+1;

    float ux = x-x0;
    float uy = y-y0;

    QVector2D pos(x,y);
    QVector2D d0 = pos-QVector2D(x0,y0);
    QVector2D d1 = pos-QVector2D(x1,y0);
    QVector2D d2 = pos-QVector2D(x0,y1);
    QVector2D d3 = pos-QVector2D(x1,y1);

    QVector2D g0 = quad[gradients[((x0%rows)*columns + (y0%columns))]];
    QVector2D g1 = quad[gradients[((x1%rows)*columns + (y0%columns))]];
    QVector2D g2 = quad[gradients[((x0%rows)*columns + (y1%columns))]];
    QVector2D g3 = quad[gradients[((x1%rows)*columns + (y1%columns))]];

    float p0 = QVector2D::dotProduct(d0, g0);
    float p1 = QVector2D::dotProduct(d1, g1);
    float p2 = QVector2D::dotProduct(d2, g2);
    float p3 = QVector2D::dotProduct(d3, g3);

    float rx = fade(ux);
    float ry = fade(uy);
    float m0 = mix(p0, p1, rx);
    float m1 = mix(p2, p3, rx);

    float res = mix(m0,m1,ry);

    /*if (first) {
        mn = mx = res;
        first = false;
    }
    else {
        if (res < mn) mn = res;
        else if (res > mx) mx = res;
    }*/

    return (res+1)/2;
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::mix(float a, float b, float r) {
    return (1-r)*a + r*b;
}

float PerlinNoise::octavePerlin(float x, float y, int octaves, double persistence) const {
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;
    for(int i=0;i<octaves;i++) {
        total += perlinSample(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total/maxValue;
}

float* PerlinNoise::genPerlinTexture(int w, int h, float offset, float speed, int octaves, float persistence) const {
    float* tex = new float[w*h];

    float incrX = speed;
    float incrY = speed;

    float offY = offset + incrY/2;

    for (int i = 0; i < h; ++i) {
        float offX = offset + incrX/2;
        for (int j = 0; j < w; ++j) {
            int addr = (i*w + j);
            float perl = octavePerlin(offX, offY, octaves, persistence);

            tex[addr] = perl;
            offX += incrX;
        }
        offY += incrY;
    }
    //cout << "Min " << mn << " max " << mx << endl;
    return tex;
}
