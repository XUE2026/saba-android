#include "GLES_ShaderUtil.h"
#include "GLShaderUtil.h"
#include <Saba/Base/Path.h>
#include <Saba/Base/File.h>
#include <Saba/Base/Log.h>

namespace saba
{

	void GLES_ShaderUtil::SetShaderDir(const std::string& shaderDir)
	{
		m_shaderDir = shaderDir;
	}

	void GLES_ShaderUtil::SetGLSLDefine(const GLSLDefine& define)
	{
		m_define = define;
	}

	void GLES_ShaderUtil::SetGLSLInclude(const GLSLInclude& include)
	{
		m_include = include;
	}

	GLProgramObject GLES_ShaderUtil::CreateProgram(const char* shaderName)
	{
		std::string glesShaderName = std::string(shaderName) + "_gles";
		std::string shaderFilePath = PathUtil::Combine(m_shaderDir, glesShaderName);
		std::string vsFilePath = shaderFilePath + ".vert";
		std::string fsFilePath = shaderFilePath + ".frag";

		std::string vsCode;
		{
			SABA_INFO("GLES Vertex Shader File Open. {}", vsFilePath);
			TextFileReader glslFile(vsFilePath);
			if (!glslFile.IsOpen())
			{
				SABA_WARN("Open fail.");
				return GLProgramObject();
			}
			vsCode = glslFile.ReadAll();
		}

		std::string fsCode;
		{
			SABA_INFO("GLES Fragment Shader File Open. {}", fsFilePath);
			TextFileReader glslFile(fsFilePath);
			if (!glslFile.IsOpen())
			{
				SABA_WARN("Open fail.");
				return GLProgramObject();
			}
			fsCode = glslFile.ReadAll();
		}

		return CreateProgram(vsCode.c_str(), fsCode.c_str());
	}

	GLProgramObject GLES_ShaderUtil::CreateProgram(const char* vsCode, const char* fsCode)
	{
		GLSLInclude include = m_include;
		if (!m_shaderDir.empty())
		{
			include.AddInclude(m_shaderDir);
		}

		std::string ppMessage;
		std::string ppVsCode;
		bool ret = PreprocessGLSL(
			&ppVsCode,
			GLSLShaderLang::Vertex,
			vsCode,
			m_define,
			include,
			&ppMessage);
		if (!ret)
		{
			std::cout << "preprocess fail.\n";
			std::cout << ppMessage;
			return GLProgramObject();
		}

		std::string ppFsCode;
		ret = PreprocessGLSL(
			&ppFsCode,
			GLSLShaderLang::Fragment,
			fsCode,
			m_define,
			include,
			&ppMessage);
		if (!ret)
		{
			std::cout << "preprocess fail.\n";
			std::cout << ppMessage;
			return GLProgramObject();
		}

		return CreateShaderProgram(ppVsCode.c_str(), ppFsCode.c_str());
	}

}