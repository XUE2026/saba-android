#ifndef SABA_GL_GLES_SHADERUTIL_H_
#define SABA_GL_GLES_SHADERUTIL_H_

#include "GLObject.h"
#include "GLSLUtil.h"

#include <string>

namespace saba
{

	class GLES_ShaderUtil
	{
	public:
		GLES_ShaderUtil() = default;

		void SetShaderDir(const std::string& shaderDir);
		void SetGLSLDefine(const GLSLDefine& define);
		void SetGLSLInclude(const GLSLInclude& include);

		GLProgramObject CreateProgram(const char* shaderName);
		GLProgramObject CreateProgram(const char* vsCode, const char* fsCode);

	private:
		std::string	m_shaderDir;
		GLSLDefine	m_define;
		GLSLInclude	m_include;
	};

}

#endif