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

void PerlinNoise::inititalizeGradrient() {
    gradients = vector<QVector2D>(rows*columns);
    float sqrt2 = sqrt(2);
    QVector2D quad[] = {
        QVector2D(1,1),
        QVector2D(-1,1),
        QVector2D(1,-1),
        QVector2D(-1,-1),
        QVector2D(sqrt2, 0),
        QVector2D(0, sqrt2),
        QVector2D(-sqrt2, 0),
        QVector2D(0, -sqrt2)
    };

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            gradients[i*columns + j] = quad[rand()%8];
            /*float a = (rand()%10000)/float(1000);
            a = a - int(a);
            float b = (rand()%10000)/float(1000);
            b = b - int(b);
            gradients[i*columns + j] = QVector2D((rand() - rand())+a, (rand()-rand())+b).normalized();*/
        }
    }
}

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

    float p0 = QVector2D::dotProduct(d0, gradients[(x0*columns + y0)]);
    float p1 = QVector2D::dotProduct(d1, gradients[(x1*columns + y0)]);
    float p2 = QVector2D::dotProduct(d2, gradients[(x0*columns + y1)]);
    float p3 = QVector2D::dotProduct(d3, gradients[(x1*columns + y1)]);

    float rx = fade(ux);
    float ry = fade(uy);
    float m0 = mix(p0, p1, rx);
    float m1 = mix(p2, p3, rx);

    return (mix(m0, m1, ry)+1)/2;
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

float* PerlinNoise::genPerlinTexture(int w, int h) const {
    float* tex = new float[w*h];

    float wf = w;
    float hf = h;
    float hw = 1/(3*wf);
    float hh = 1/(3*hf);

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int addr = (i*w + j);
            float coordx = j/wf + hw;
            float coordy = i/hf + hh;
            //float perl = octavePerlin(coordx, coordy, 7, 0.5);
            float perl = perlinSample(coordx, coordy);
            float n = 20 * perl;
            n = n - floor(n);

            tex[addr] = n;
        }
    }

    return tex;
}
