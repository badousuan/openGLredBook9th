

#include "vapp.h"
#include "vutils.h"

#include "vmath.h"

#include "LoadShaders.h"

#include <stdio.h>
#include <string>
using namespace vmath;

#define POINT_COUNT 4


static void checkGLError(const char* cmd){
    int error = glGetError();
    std::string err;
    if( error!= GL_NO_ERROR){
        switch (error) {
            case GL_INVALID_ENUM:
                err = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                err = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                err = "GL_INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                err = "GL_OUT_OF_MEMORY";
                break;
            default:
                err = "unknow error";
                break;
        }
        printf("%s error=%s(%d)\n",cmd,err.c_str(),error);
    };
}


BEGIN_APP_DECLARATION(PointSpriteExample)
    // Override functions from base class
    virtual void Initialize(const char * title);
    virtual void Display(bool auto_redraw);
    virtual void Finalize(void);
    virtual void Resize(int width, int height);

    // Member variables
    float aspect;
    GLuint render_prog;
    GLuint vao[1];
    GLuint vbo[1];
    GLuint sprite_texture;

    GLint render_model_matrix_loc;
    GLint render_projection_matrix_loc;
END_APP_DECLARATION()

DEFINE_APP(PointSpriteExample, "Point Sprite Example")

namespace vtarga {
unsigned char * load_targa(const char * filename, GLenum &format, int &width, int &height);
};

static inline float random_float()
{
    float res;
    unsigned int tmp;
    static unsigned int seed = 0x13371337;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

void PointSpriteExample::Initialize(const char * title)
{
    base::Initialize(title);

    GLenum format = GL_RGBA;
    int width = 128, height = 128;
    uint8_t* data = NULL;

    glGenTextures(1, &sprite_texture);
    checkGLError("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, sprite_texture);
    checkGLError("glBindTexture");
    data = vtarga::load_targa("media/sprite2.tga", format, width, height);
    //height = 32;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	checkGLError("glBindTexture");
    //glTexStorage2D 在openGL 4.2才出现
    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    checkGLError("glTexStorage2D");
    //internal format 不能用 GL_RGBA8，除非加上上一句
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA ,width, height, 0, format , GL_UNSIGNED_BYTE, data);
    checkGLError("glTexImage2D");
    delete [] data;

    glGenerateMipmap(GL_TEXTURE_2D);

    static ShaderInfo shader_info[] =
    {
        { GL_VERTEX_SHADER, "media/shaders/pointsprites/pointsprites.vs.glsl", 0 },
        { GL_FRAGMENT_SHADER, "media/shaders/pointsprites/pointsprites.fs.glsl", 0 },
        { GL_NONE, NULL, 0 }
    };
    checkGLError("glGenerateMipmap");
    render_prog = LoadShaders(shader_info);
    checkGLError("LoadShaders");
    glUseProgram(render_prog);
    checkGLError("glUseProgram");
    // "model_matrix" is actually an array of 4 matrices
    render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
    render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

    // A single triangle
    static vec4 * vertex_positions;

    // Set up the vertex attributes
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, POINT_COUNT * sizeof(vec4), NULL, GL_STATIC_DRAW);

    vertex_positions = (vec4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int n = 0; n < POINT_COUNT; n++)
    {
        vertex_positions[n] = vec4(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, 1.0f);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PointSpriteExample::Display(bool auto_redraw)
{
    float t = float(app_time() & 0x1FFF) / float(0x1FFF);
    static float q = 0.0f;
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    mat4 model_matrix;

    // Setup
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate simple shading program
    glUseProgram(render_prog);

    // Set up the model and projection matrix
    mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 8.0f));
    glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    glBindTexture(GL_TEXTURE_2D, sprite_texture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(32.0f);

    // Draw Arrays...
    model_matrix = translate(0.0f, 0.0f, -2.0f) *
                   rotate(t * 360.0f, Y) * rotate(t * 720.0f, Z);
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
    glDrawArrays(GL_POINTS, 0, POINT_COUNT);

    base::Display();
}

void PointSpriteExample::Finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(render_prog);
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);
}

void PointSpriteExample::Resize(int width, int height)
{
    glViewport(0, 0 , width, height);

    aspect = float(height) / float(width);
}
