#include "../cEffect.h"
#include "../sContext.h" // To access the OpenGL context
#include "../cShader.h" // For shader loading and management
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

namespace eae6320
{
    namespace Graphics
    {
        cResult cEffect::Initialize()
        {
			auto result = eae6320::Results::Success;

			if (!(result = eae6320::Graphics::cShader::Load("data/Shaders/Vertex/standard.shader",
				m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
			{
				EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
				return result;
			}
			if (!(result = eae6320::Graphics::cShader::Load("data/Shaders/Fragment/animatedColor.shader",
				m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
			{
				EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
				return result;
			}
			{
				constexpr auto renderStateBits = []
				{
					uint8_t renderStateBits = 0;

					eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
					eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
					eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
					eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

					return renderStateBits;
				}();
				if (!(result = m_renderState.Initialize(renderStateBits)))
				{
					EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
					return result;
				}
			}

			// Create a program
			eae6320::cScopeGuard scopeGuard_program([this,&result]
				{
					if (!result)
					{
						if (m_programId != 0)
						{
							glDeleteProgram(m_programId);
							const auto errorCode = glGetError();
							if (errorCode != GL_NO_ERROR)
							{
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
							}
							m_programId = 0;
						}
					}
				});
			{
				EAE6320_ASSERT(m_programId == 0);
				m_programId = glCreateProgram();
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
				else if (m_programId == 0)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("OpenGL failed to create a program");
					return result;
				}
			}
			// Attach the shaders to the program
			{
				// Vertex
				{
					EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderId != 0));
					glAttachShader(m_programId, m_vertexShader->m_shaderId);
					const auto errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to attach the vertex shader to the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return result;
					}
				}
				// Fragment
				{
					EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderId != 0));
					glAttachShader(m_programId, m_fragmentShader->m_shaderId);
					const auto errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to attach the fragment shader to the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return result;
					}
				}
			}
			// Link the program
			{
				glLinkProgram(m_programId);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					// Get link info
					// (this won't be used unless linking fails
					// but it can be useful to look at when debugging)
					std::string linkInfo;
					{
						GLint infoSize;
						glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoSize);
						const auto errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							if (infoSize > 0)
							{
								auto* const info = new (std::nothrow) GLchar[infoSize];
								if (info)
								{
									eae6320::cScopeGuard scopeGuard_info([info]
										{
											delete[] info;
										});
									constexpr GLsizei* const dontReturnLength = nullptr;
									glGetProgramInfoLog(m_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info);
									const auto errorCode = glGetError();
									if (errorCode == GL_NO_ERROR)
									{
										linkInfo = info;
									}
									else
									{
										result = eae6320::Results::Failure;
										EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
										eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
											reinterpret_cast<const char*>(gluErrorString(errorCode)));
										return result;
									}
								}
								else
								{
									result = eae6320::Results::OutOfMemory;
									EAE6320_ASSERTF(false, "Couldn't allocate memory for the program link info");
									eae6320::Logging::OutputError("Failed to allocate memory for the program link info");
									return result;
								}
							}
						}
						else
						{
							result = eae6320::Results::Failure;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							return result;
						}
					}
					// Check to see if there were link errors
					GLint didLinkingSucceed;
					{
						glGetProgramiv(m_programId, GL_LINK_STATUS, &didLinkingSucceed);
						const auto errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							if (didLinkingSucceed == GL_FALSE)
							{
								result = eae6320::Results::Failure;
								EAE6320_ASSERTF(false, linkInfo.c_str());
								eae6320::Logging::OutputError("The program failed to link: %s",
									linkInfo.c_str());
								return result;
							}
						}
						else
						{
							result = eae6320::Results::Failure;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							return result;
						}
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}

			return result;
        }

        void cEffect::Bind() const
        {
			EAE6320_ASSERT(m_programId != 0);
			glUseProgram(m_programId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			m_renderState.Bind();
        }

        cResult cEffect::CleanUp()
        {
			auto result = eae6320::Results::Success;

			if (m_programId != 0)
			{
				glDeleteProgram(m_programId);
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					if (result)
					{
						result = eae6320::Results::Failure;
					}
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				m_programId = 0;
			}
			if (m_vertexShader)
			{
				m_vertexShader->DecrementReferenceCount();
				m_vertexShader = nullptr;
			}
			if (m_fragmentShader)
			{
				m_fragmentShader->DecrementReferenceCount();
				m_fragmentShader = nullptr;
			}
			return result;
        }
    }
}
