#ifndef _MAIN_H
#define _MAIN_H

#include "basicplugin.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>

class PerlinPlugin : public QObject, public BasicPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "BasicPlugin")
    Q_INTERFACES(BasicPlugin)

public:
    void onPluginLoad();
    bool paintGL();
private:
    //float* tex;
    QOpenGLShaderProgram* program;
    QOpenGLShader* vs;
    QOpenGLShader* fs;
    GLuint perlinTextureId;
    GLuint textureId0, textureId1;
    GLuint VAO;
    GLuint VBO;

    GLuint terrainVAO;
    GLuint terrainVertex;
    GLuint noiseVBO;
};

#endif
