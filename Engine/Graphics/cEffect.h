#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

#include <Engine/Results/Results.h>
#include "Includes.h"
#include "cShader.h"
#include "cRenderState.h"

namespace eae6320
{
    namespace Graphics
    {
        class cEffect
        {
        public:
            cResult Initialize();
            void Bind() const;
            cResult CleanUp();

        private:
            // Platform-independent data members
            cShader* m_vertexShader = nullptr;
            cShader* m_fragmentShader = nullptr;
            cRenderState m_renderState;

            // Platform-specific data members
#if defined(EAE6320_PLATFORM_GL)
            GLuint m_programId = 0;
#endif
        };
    }
}

#endif  //EAE6320_GRAPHICS_CEFFECT_H