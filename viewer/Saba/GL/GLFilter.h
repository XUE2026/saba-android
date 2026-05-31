//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_GLFILTER_H_
#define SABA_GL_GLFILTER_H_

#include "GLObject.h"

#include <string>
#include <vector>
#include <glm/vec2.hpp>

namespace saba
{
	struct FilterParams
	{
		float exposure = 1.0f;
		float contrast = 1.0f;
		float saturation = 1.0f;
		float hueShift = 0.0f;
		float temperature = 0.0f;
		float tint = 0.0f;
	};

	enum class FilterPreset
	{
		None,
		WhiteBright,
		SkinBright,
		Dark,
		GenshinDay,
		GenshinNight,
		GenshinDusk
	};

	class GLFilter
	{
	public:
		GLFilter();
		~GLFilter();

		bool Initialize(int width, int height);
		void Apply(GLuint inputTexture, FilterPreset preset, const FilterParams& customParams);
		void Resize(int width, int height);

		static FilterParams GetPresetParams(FilterPreset preset);

		GLuint GetOutputTexture() const { return m_outputTex.Get(); }

	private:
		bool CreateFBOs(int width, int height);
		bool CreateShaders();
		void ApplyFilter(GLuint inputTex, GLuint outputFbo, const FilterParams& params);
		std::string GetVertexShader() const;
		std::string GetFragmentShader() const;

		int m_width;
		int m_height;

		GLFramebufferObject m_fbo[2];
		GLTextureObject m_fboTex[2];
		GLTextureObject m_outputTex;

		GLProgramObject m_filterProgram;

		int m_pingIndex;
	};
}

#endif // !SABA_GL_GLFILTER_H_