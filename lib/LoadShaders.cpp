//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.cxx ---
//
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

#include <GL3/gl3w.h>
#include "LoadShaders.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#define _DEBUG 1
//----------------------------------------------------------------------------
static    void checkGLError(const char* cmd){
        int error =glGetError();
        if( error!= GL_NO_ERROR){
            printf("%s error=%d\n",cmd,error);
        };
    }
    
static const GLchar*
ReadShader( const char* filename )
{
#ifdef WIN32
	FILE* infile;
	fopen_s( &infile, filename, "rb" );
#else
    FILE* infile = fopen( filename, "rb" );
#endif // WIN32

    if ( !infile ) {
#ifdef _DEBUG
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    GLchar* source = new GLchar[len+1];

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return const_cast<const GLchar*>(source);
}

//----------------------------------------------------------------------------

GLuint
LoadShaders(ShaderInfo* shaders)
{
    if ( shaders == NULL ) { return 0; }

    GLuint program = glCreateProgram();
    checkGLError("glCreateProgram");
    ShaderInfo* entry = shaders;
    while ( entry->type != GL_NONE ) {
        GLuint shader = glCreateShader( entry->type );
        checkGLError("glCreateShader");
        entry->shader = shader;

        const GLchar* source = ReadShader( entry->filename );
        if ( source == NULL ) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource( shader, 1, &source, NULL );
        checkGLError("glShaderSource");
        delete [] source;

        glCompileShader( shader );
        checkGLError("glCompileShader");
        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        checkGLError("glGetShaderiv");
        if ( !compiled ) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( shader, len, &len, log );
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete [] log;
#endif /* DEBUG */

            return 0;
        }

        glAttachShader( program, shader );
         checkGLError("glAttachShader");
        ++entry;
    }

    glLinkProgram( program );

    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( program, len, &len, log );
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete [] log;
#endif /* DEBUG */

        for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }
        
        return 0;
    }

    return program;
}

//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus


