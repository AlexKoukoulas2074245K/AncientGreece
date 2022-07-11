#pragma once

#define GL_SILENCE_DEPRECATION
#ifdef _WIN32

//---------------------------------------------------------------------------
//  On Win32, use our own GL headers and prepare a function table
//---------------------------------------------------------------------------
#include "khrplatform.h"
#include "gl2platform.h"
#include "gl2.h"
#include "gl2ext.h"
#include <assert.h>

struct GLFuncTable {
#define GL_FUNC(retVal, name, args) retVal (GL_APIENTRY *name)args;
#include "Funcs.h"
#undef GL_FUNC
    void initialize();
};
extern GLFuncTable glFuncTable;
#define GL_CHECK(call) do { glFuncTable.call; assert(glFuncTable.glGetError() == GL_NO_ERROR); } while (0)
#define GL_CHECK_AGAINST_ARG(call, arg) do { assert(glFuncTable.call == arg); } while (0)
#define GL_NO_CHECK(call) (glFuncTable.call)
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_TEXTURE_BORDER_COLOR 0x1004

#else // TURF_TARGET_WIN32

//---------------------------------------------------------------------------
//  Otherwise, use the system's GL headers
//---------------------------------------------------------------------------
#ifdef __APPLE__
    #include <OpenGL/gl3.h>
#else
    #include <GLES2/gl3.h>
#endif

#include "../../common/utils/Logging.h"

#define GL_CHECK(call) do { call; auto err = glGetError(); if(err != GL_NO_ERROR){ Log(LogType::INFO, "GL ERROR %d", err); } assert(err == GL_NO_ERROR); } while (0)
#define GL_NO_CHECK(call) (call)

#endif // TURF_TARGET_WIN32
