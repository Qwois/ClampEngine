#include "ClampEngine.h"
#include "../Globals/Globals.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else
#define GLSL_VERSION 100
#endif