#include "MediaExplorerPresenter.h"
#include "PreviewPane.h"
#include "ui_PreviewPane.h"

#include <QGLFormat>

#include <libraw/libraw.h>
#include <iostream>

PreviewPane::PreviewPane(MediaExplorerPresenter* presenter, QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::PreviewPane)
{    
    ui->setupUi(this);

    _presenter = presenter;

    connect(_presenter, SIGNAL(NewDataAvailable(OCImage*)), this, SLOT(UpdateFrame(OCImage*)));

    //this->setAttribute(Qt::WA_DontCreateNativeAncestors);
}

PreviewPane::~PreviewPane()
{
    delete ui;
}

void PreviewPane::resizeGL(int w, int h)
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void PreviewPane::paintGL()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT );

    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PreviewPane::initializeGL()
{
//    QSurfaceFormat format;
//    format.setDepthBufferSize(24);
//    format.setStencilBufferSize(8);
//    format.setVersion(3, 3);
//    format.setProfile(QSurfaceFormat::CoreProfile);
//    this->setFormat(format);

    initializeOpenGLFunctions();

    const GLubyte* test = glGetString(GL_VERSION);
    std::cout << test << std::endl;
    CreateRectangle();

    // Set the clear color to black, not black atm for debugging purposes, will eventually become user setting later
    glClearColor( 1.0f / 255.0f * 18.0f, 1.0f / 255.0f * 115.0f, 1.0f / 255.0f * 166.0f, 1.0f );
}

//Reference: https://open.gl/drawing, https://open.gl/textures
void PreviewPane::CreateRectangle()
{
    glGenBuffers(1, &rectangleVBO); // Generate 1 buffer
    glGenBuffers(1, &rectangleUV); // Generate 1 buffer

    //TODO: UV flipped upside down to get correct presentation, investigate later
    float rectangleData[] =
    {
    //  Position      Texcoords
        -1.0f, -1.0f, 0.0f, 1.0f, // Top-left
         1.0f, -1.0f, 1.0f, 1.0f, // Top-right
        -1.0f,  1.0f, 0.0f, 0.0f, // Bottom-right
         1.0f,  1.0f, 1.0f, 0.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleData), rectangleData, GL_STATIC_DRAW);

    CreateShaders();

    //Setup buffers

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "vertTexCoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    //glEnable(GL_TEXTURE_2D);
    //Setup texture
    glGenTextures(1, &tex);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, tex);

    // Creation of image processing object
    LibRaw imageProcessor;
    imageProcessor.open_file("000100.dng");
    imageProcessor.unpack();
    imageProcessor.dcraw_process();
    libraw_processed_image_t* image = imageProcessor.dcraw_make_mem_image();

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, &imageProcessor.imgdata.rawdata.raw_image /*image->data*/);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr/* &image->data*/);

    imageProcessor.dcraw_clear_mem(image);
    imageProcessor.recycle();

    // Black/white checkerboard
    /*float pixels[] = {
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    };*/
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    //shaderProgram->setUniform("texture1", 0);
}

//Hardcoded, shader manager is not available atm
void PreviewPane::CreateShaders()
{
    std::string vertexShaderSource(
            "#version 130 \n\
            in vec2 position; \n\
            in vec2 vertTexCoord;\n\
            out vec2 fragTexCoord;\n\
            void main() \n\
            { \n\
                fragTexCoord = vertTexCoord; \n\
                gl_Position = vec4(position, 0.0, 1.0); \n\
            } \n\
            ");

    std::string fragmentShaderSource(
        "#version 130 \n\
        uniform sampler2D texture1; \n\
        in vec2 fragTexCoord; \n\
        out vec3 outColor; \n\
        void main() \n\
        { \n\
            outColor = texture( texture1, fragTexCoord ).rgb;// * vec3(0.0, 2.4, 0.0); \n\
        } \n\
        ");

    const char* tempShader = vertexShaderSource.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &tempShader, NULL);
    glCompileShader(vertexShader);

    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

    char buffer[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);

    tempShader = fragmentShaderSource.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &tempShader, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
}

void PreviewPane::UpdateFrame(OCImage *image)
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, tex);

    int width = image->GetWidth();
    int height = image->GetHeight();

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image->GetData());

    GLenum error = glGetError();

    glBindTexture(GL_TEXTURE_2D, 0);

    this->update();
}
