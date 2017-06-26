#include "main.h"
#include <QCoreApplication>
#include "perlin.h"

const int WIDTH = 1024;
const int HEIGHT = WIDTH;

const int meshCols = 30;
const int meshRows = 30;

const int rowVertices = 4 + (meshCols-1)*2;

void PerlinPlugin::onPluginLoad() {
    cout << "Generating perlin object" << endl;
    PerlinNoise perlin(1024, 1024, 0);
    cout << "Getting texture" << endl;
    float* tex = perlin.genPerlinTexture(WIDTH, HEIGHT);

    GLWidget & g = *glwidget();
    g.makeCurrent();
    // Carregar shader, compile & link
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(QCoreApplication::applicationDirPath()+
                          "/../../plugins/perlin_noise/terrain.vert");

    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(QCoreApplication::applicationDirPath()+
                          "/../../plugins/perlin_noise/terrain.frag");

    program = new QOpenGLShaderProgram(this);
    program->addShader(vs);
    program->addShader(fs);
    program->link();

    // Setup texture
    /*g.glActiveTexture(GL_TEXTURE0);
    g.glGenTextures(1, &textureId);
    g.glBindTexture(GL_TEXTURE_2D, textureId);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, tex);
    g.glBindTexture(GL_TEXTURE_2D, 0);*/

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

    vector<float> terrainRow(rowVertices * 2);

    for (unsigned int i = 0, x = 0; i < terrainRow.size(); i += 4, ++x) {
        terrainRow[i] = x;
        terrainRow[i+1] = 0;

        terrainRow[i+2] = x;
        terrainRow[i+3] = 1;
    }

    vector<float> perlinNoise(rowVertices*meshRows);

    float offY = 0.001;
    float incr = 0.05;
    for (unsigned int j = 0; j < meshRows; ++j, offY += incr) {
        float offX = 0.001;
        for (unsigned int i = 0; i < rowVertices; i += 2, offX += incr) {
            perlinNoise[j*rowVertices + i] = perlin.octavePerlin(offX, offY, 7, 0.5);
            perlinNoise[j*rowVertices + i+1] = perlin.octavePerlin(offX, offY + incr, 7, 0.5);
        }
    }

    /*for (int i = 0; i < perlinNoise.size(); ++i) {
        cout << perlinNoise[i] << " ";
    }
    cout << endl;*/

    g.glGenVertexArrays(1, &terrainVAO);
    g.glGenBuffers(1, &terrainVertex);

    g.glBindVertexArray(terrainVAO);
    g.glBindBuffer(GL_ARRAY_BUFFER, terrainVertex);
    g.glBufferData(GL_ARRAY_BUFFER, sizeof(float)*terrainRow.size(), &terrainRow[0], GL_STATIC_DRAW);
    g.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    g.glEnableVertexAttribArray(0);

    g.glGenBuffers(1, &noiseVBO);
    g.glBindBuffer(GL_ARRAY_BUFFER, noiseVBO);
    g.glBufferData(GL_ARRAY_BUFFER, sizeof(float)*perlinNoise.size(), &perlinNoise[0], GL_STATIC_DRAW);
    g.glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    g.glEnableVertexAttribArray(1);

    g.glBindBuffer(GL_ARRAY_BUFFER, 0);
    g.glBindVertexArray(0);
}

bool PerlinPlugin::paintGL() {
    GLWidget &g = *glwidget();
    g.makeCurrent();
    g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    //g.glBindTexture(GL_TEXTURE_2D, textureId);


    //model.translate(-15,-0.5,0);
    program->bind();
    //program->setUniformValue("tex", 0);
    g.glBindVertexArray(terrainVAO);
    camera()->setZfar(1000);
    camera()->setZnear(0.1);
    for (int i = 0; i < meshRows; ++i) {
        QMatrix4x4 model;
        model.translate(0,i,0);
        program->setUniformValue("modelViewMatrix", camera()->viewMatrix()*model);
        program->setUniformValue("modelViewProjectionMatrix", camera()->projectionMatrix() * camera()->viewMatrix()*model);
        g.glBindBuffer(GL_ARRAY_BUFFER, noiseVBO);
        g.glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0,(void*) (i*rowVertices*sizeof(float)));
        g.glDrawArrays(GL_TRIANGLE_STRIP, 0, rowVertices);
    }
    g.glBindBuffer(GL_ARRAY_BUFFER, 0);
    g.glBindVertexArray(0);
    program->release();
    //g.glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
