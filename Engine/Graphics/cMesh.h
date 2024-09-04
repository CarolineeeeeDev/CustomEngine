#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

#include <Engine/Results/Results.h>
#include "Includes.h"
#include "cShader.h"
#include "cRenderState.h"

namespace eae6320
{
    namespace Graphics
    {
        class cMesh
        {
        public:
            cResult Initialize();
            void Draw() const;
            cResult CleanUp();

        private:
            // Platform-independent data members

            // Platform-specific data members
#if defined(EAE6320_PLATFORM_D3D)
            cVertexFormat* m_vertexFormat = nullptr;
            ID3D11Buffer* m_vertexBuffer = nullptr;
#elif defined(EAE6320_PLATFORM_GL)
            GLuint m_vertexBufferId = 0;
            GLuint m_vertexArrayId = 0;
#endif
        };
    }
}

#endif  //EAE6320_GRAPHICS_CMESH_H