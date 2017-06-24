#ifndef GOODPERLIN_H
#define GOODPERLIN_H


class GoodPerlin {

    static int* p;
    static int permutation[];

    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);

public:
    GoodPerlin();

    double noise(double x, double y, double z);
};

#endif // GOODPERLIN_H
