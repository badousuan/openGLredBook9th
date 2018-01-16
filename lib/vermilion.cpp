#include "vapp.h"

#include <time.h>
#include <string>
#include <iostream>
void VermilionApplication::window_size_callback(GLFWwindow* window, int width, int height)
{
    VermilionApplication* pThis = (VermilionApplication*)glfwGetWindowUserPointer(window);

    pThis->Resize(width, height);
}

void VermilionApplication::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    VermilionApplication* pThis = (VermilionApplication*)glfwGetWindowUserPointer(window);

    pThis->OnKey(key, scancode, action, mods);
}

void VermilionApplication::char_callback(GLFWwindow* window, unsigned int codepoint)
{
    VermilionApplication* pThis = (VermilionApplication*)glfwGetWindowUserPointer(window);

    pThis->OnChar(codepoint);
}

unsigned int VermilionApplication::app_time()
{
#ifdef _WIN32
    ULONGLONG currentTime = ::GetTickCount64();

    return (unsigned int)(currentTime - m_appStartTime);
#else
    return 0;
#endif
}

void VermilionApplication::Initialize(const char * title)
{
#ifdef _WIN32
    m_appStartTime = ::GetTickCount64();
#else
    gettimeofday(&m_appStartTime, nullptr);
#endif

    glfwInit();
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    m_pWindow = glfwCreateWindow(800, 600, title ? title : "OpenGL Application", nullptr, nullptr);
    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetWindowSizeCallback(m_pWindow, window_size_callback);
    glfwSetKeyCallback(m_pWindow, key_callback);
    glfwSetCharCallback(m_pWindow, char_callback);

    glfwMakeContextCurrent(m_pWindow);

    gl3wInit();
     

    Resize(800, 600);

#ifdef _DEBUG
    if (glDebugMessageCallbackARB != NULL)
        glDebugMessageCallbackARB(DebugOutputCallback, this);
#endif
    std::cout<<"vendor:"<<glGetString(GL_VENDOR)<<"\nversion:"<< glGetString(GL_VERSION)<<"\nrender:"<<glGetString(GL_RENDERER)<<std::endl;
    
    
}
