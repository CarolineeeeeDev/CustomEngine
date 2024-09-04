#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>

#ifdef EAE6320_PLATFORM_D3D
#include <d3d11.h>
#include <dxgi.h>
#include "cVertexFormat.h"

#else ifdef EAE6320_PLATFORM_GL
#include <gl/GL.h>
#include <gl/GLU.h>
#include <new>
#include <External/OpenGlExtensions/OpenGlExtensions.h>
#endif

#endif