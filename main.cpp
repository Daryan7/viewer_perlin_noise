#include "main.h"
#include <QCoreApplication>
#include "perlin.h"

const int WIDTH = 1024;
const int HEIGHT = WIDTH;

const int meshCols = 100;
const int meshRows = 100;

const int rowVertices = 4 + (meshCols-1)*2;

void PerlinPlugin::onPluginLoad() {
    octaves = 8;
    persistence = 0.5;
    offset = 0;
    speed = 0.01;

    perlin = PerlinNoise(1024, 1024, 0);

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


    vsPerlinTex = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vsPerlinTex->compileSourceFile(QCoreApplication::applicationDirPath()+
                                   "/../../plugins/perlin_noise/perlinTex.vert");

    fsPerlinTex = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fsPerlinTex->compileSourceFile(QCoreApplication::applicationDirPath()+
                                   "/../../plugins/perlin_noise/perlinTex.frag");

    programPerlinTex = new QOpenGLShaderProgram(this);
    programPerlinTex->addShader(vsPerlinTex);
    programPerlinTex->addShader(fsPerlinTex);
    programPerlinTex->link();

    // Setup texture
    g.glActiveTexture(GL_TEXTURE0);
    g.glGenTextures(1, &perlinTextureId);

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

    g.glGenVertexArrays(1, &terrainVAO);
    g.glGenBuffers(1, &terrainVertex);
    g.glGenBuffers(1, &noiseVBO);

    g.glBindVertexArray(terrainVAO);
    g.glBindBuffer(GL_ARRAY_BUFFER, terrainVertex);
    g.glBufferData(GL_ARRAY_BUFFER, sizeof(float)*terrainRow.size(), &terrainRow[0], GL_STATIC_DRAW);
    g.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    g.glEnableVertexAttribArray(0);

    g.glBindBuffer(GL_ARRAY_BUFFER, 0);
    g.glBindVertexArray(0);

    genTerrain();

    QImage img0(QCoreApplication::applicationDirPath()+
                "/../../plugins/perlin_noise/grass.png");
    QImage im0 = img0.convertToFormat(QImage::Format_ARGB32).rgbSwapped().mirrored();
    g.glActiveTexture(GL_TEXTURE0);
    g.glGenTextures( 1, &textureId0);
    g.glBindTexture(GL_TEXTURE_2D, textureId0);
    g.glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, im0.width(), im0.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im0.bits());
    g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    g.glBindTexture(GL_TEXTURE_2D, 0);

    QImage img1(QCoreApplication::applicationDirPath()+
                "/../../plugins/perlin_noise/rock.png");
    QImage im1 = img1.convertToFormat(QImage::Format_ARGB32).rgbSwapped().mirrored();
    g.glActiveTexture(GL_TEXTURE1);
    g.glGenTextures( 1, &textureId1);
    g.glBindTexture(GL_TEXTURE_2D, textureId1);
    g.glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, im1.width(), im1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im1.bits());
    g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    g.glBindTexture(GL_TEXTURE_2D, 0);

    g.resize(750, 750);
}

bool PerlinPlugin::paintGL() {
    GLWidget &g = *glwidget();
    g.makeCurrent();
    g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    programPerlinTex->bind();
    programPerlinTex->setUniformValue("tex", 0);
    g.glActiveTexture(GL_TEXTURE0);
    g.glBindTexture(GL_TEXTURE_2D, perlinTextureId);
    g.glBindVertexArray(VAO);
    g.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    programPerlinTex->release();


    program->bind();
    program->setUniformValue("grass", 0);
    program->setUniformValue("rock", 1);
    g.glActiveTexture(GL_TEXTURE0);
    g.glBindTexture(GL_TEXTURE_2D, textureId0);
    g.glActiveTexture(GL_TEXTURE1);
    g.glBindTexture(GL_TEXTURE_2D, textureId1);

    g.glBindVertexArray(terrainVAO);
    camera()->setZfar(1000);
    camera()->setZnear(0.1);

    for (int i = 0; i < meshRows; ++i) {
        QMatrix4x4 model;
        model.translate(-meshCols/2,-meshRows/2,0);
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

    g.glActiveTexture(GL_TEXTURE0);
    g.glBindTexture(GL_TEXTURE_2D, 0);
    g.glActiveTexture(GL_TEXTURE1);
    g.glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void PerlinPlugin::genTerrain() {
    float* tex = perlin.genPerlinTexture(WIDTH, HEIGHT, offset, speed, octaves, persistence);
    GLWidget& g = *glwidget();
    g.makeCurrent();

    g.glActiveTexture(GL_TEXTURE0);
    g.glBindTexture(GL_TEXTURE_2D, perlinTextureId);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, tex);
    g.glBindTexture(GL_TEXTURE_2D, 0);
    delete tex;

    vector<float> perlinNoise(rowVertices*meshRows);

    float offY = offset + speed/2;
    for (unsigned int j = 0; j < meshRows; ++j, offY += speed) {
        float offX = offset + speed/2;
        for (unsigned int i = 0; i < rowVertices; i += 2, offX += speed) {
            perlinNoise[j*rowVertices + i] = perlin.octavePerlin(offX, offY, octaves, persistence);
            perlinNoise[j*rowVertices + i+1] = perlin.octavePerlin(offX, offY + speed, octaves, persistence);
        }
    }

    g.glBindVertexArray(terrainVAO);
    g.glBindBuffer(GL_ARRAY_BUFFER, noiseVBO);
    g.glBufferData(GL_ARRAY_BUFFER, sizeof(float)*perlinNoise.size(), &perlinNoise[0], GL_STATIC_DRAW);
    g.glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    g.glEnableVertexAttribArray(1);

    g.glBindVertexArray(0);
    g.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PerlinPlugin::keyPressEvent(QKeyEvent *e) {
    switch(e->key()) {
    case Qt::Key_N:
        perlin = PerlinNoise(1024, 1024);
        break;
    case Qt::Key_Right:
        speed += 0.003;
        break;
    case Qt::Key_Left:
        if (speed - 0.003 >= 0) {
            speed -= 0.003;
        }
        break;
    case Qt::Key_Up:
        persistence += 0.02;
        break;
    case Qt::Key_Down:
        if (persistence - 0.02 >= 0) {
            persistence -= 0.02;
        }
        break;
    case Qt::Key_Q:
        if (offset - 1 >= 0) {
            offset -= 1;
        }
        break;
    case Qt::Key_E:
        offset += 1;
        break;
    case Qt::Key_Z:
        if (octaves - 1 >= 0) {
            --octaves;
        }
        break;
    case Qt::Key_C:
        ++octaves;
        break;
    default:
        if (e->key() == Qt::Key_P) {
            cout << "Parameters: " << endl;
            cout << "Speed: " << speed << endl;
            cout << "Persistence: " << persistence << endl;
            cout << "Offset: " << offset << endl;
            cout << "Octaves: " << octaves << endl;
        }
        return;
    }
    genTerrain();
    glwidget()->update();
}
