#include "main.h"
#include <QCoreApplication>
#include "perlin.h"

const int WIDTH = 1024;
const int HEIGHT = WIDTH;

void PerlinPlugin::onPluginLoad() {
    cout << "Generating perlin object" << endl;
    PerlinNoise perlin(512, 512);
    cout << "Getting texture" << endl;
    float* tex = perlin.genPerlinTexture(WIDTH, HEIGHT);

    GLWidget & g = *glwidget();
    g.makeCurrent();
    // Carregar shader, compile & link
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(QCoreApplication::applicationDirPath()+
                          "/../../plugins/perlin_noise/shader.vert");

    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(QCoreApplication::applicationDirPath()+
                          "/../../plugins/perlin_noise/shader.frag");

    program = new QOpenGLShaderProgram(this);
    program->addShader(vs);
    program->addShader(fs);
    program->link();

    // Setup texture
    g.glActiveTexture(GL_TEXTURE0);
    g.glGenTextures(1, &textureId);
    g.glBindTexture(GL_TEXTURE_2D, textureId);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, tex);
    g.glBindTexture(GL_TEXTURE_2D, 0);

    float vertices[] = {
        0,0,0,
        1,0,0,
        0,1,0,
        1,1,0
    };

    g.glGenVertexArrays(1, &VAO);
    g.glGenBuffers(1, &VBO);

    g.glBindVertexArray(VAO);

    g.glBindBuffer(GL_ARRAY_BUFFER, VBO);
    g.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    g.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    g.glEnableVertexAttribArray(0);
}

bool PerlinPlugin::paintGL() {
    GLWidget &g = *glwidget();
    g.makeCurrent();
    g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    g.glBindTexture(GL_TEXTURE_2D, textureId);

    program->bind();
    program->setUniformValue("tex", 0);
    g.glBindVertexArray(VAO);
    g.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    g.glBindVertexArray(0);
    program->release();
    g.glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
