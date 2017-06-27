#ifndef _MAIN_H
#define _MAIN_H

#include "basicplugin.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>
#include "perlin.h"

class PerlinPlugin : public QObject, public BasicPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "BasicPlugin")
    Q_INTERFACES(BasicPlugin)

public:
    void onPluginLoad();
    bool paintGL();
    void keyPressEvent(QKeyEvent *);
private:

    void genTerrain();

    int octaves;
    float persistence;
    float offset;
    float speed;

    PerlinNoise perlin;
    QOpenGLShaderProgram* program, *programPerlinTex;
    QOpenGLShader* vs, *vsPerlinTex;
    QOpenGLShader* fs, *fsPerlinTex;
    GLuint perlinTextureId;
    GLuint textureId0, textureId1;
    GLuint VAO;
    GLuint VBO;

    GLuint terrainVAO;
    GLuint terrainVertex;
    GLuint noiseVBO;
};

#endif
