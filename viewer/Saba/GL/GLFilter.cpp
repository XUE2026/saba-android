//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GLFilter.h"

#include <GL/gl3w.h>
#include <cmath>
#include <cstdio>

namespace saba
{
	GLFilter::GLFilter()
		: m_width(0)
		, m_height(0)
		, m_pingIndex(0)
	{
	}

	GLFilter::~GLFilter()
	{
	}

	bool GLFilter::Initialize(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_pingIndex = 0;

		if (!CreateShaders())
		{
			return false;
		}

		if (!CreateFBOs(width, height))
		{
			return false;
		}

		return true;
	}

	bool GLFilter::CreateShaders()
	{
		std::string vsCode = GetVertexShader();
		std::string fsCode = GetFragmentShader();

		const char* vsSrc = vsCode.c_str();
		const char* fsSrc = fsCode.c_str();

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vsSrc, nullptr);
		glCompileShader(vs);
		GLint compiled = 0;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			char log[512];
			glGetShaderInfoLog(vs, sizeof(log), nullptr, log);
			glDeleteShader(vs);
			return false;
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fsSrc, nullptr);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			char log[512];
			glGetShaderInfoLog(fs, sizeof(log), nullptr, log);
			glDeleteShader(vs);
			glDeleteShader(fs);
			return false;
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		GLint linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			char log[512];
			glGetProgramInfoLog(program, sizeof(log), nullptr, log);
			glDeleteShader(vs);
			glDeleteShader(fs);
			glDeleteProgram(program);
			return false;
		}

		glDeleteShader(vs);
		glDeleteShader(fs);

		m_filterProgram.Reset(program);
		return true;
	}

	bool GLFilter::CreateFBOs(int width, int height)
	{
		for (int i = 0; i < 2; i++)
		{
			if (!m_fboTex[i].Create())
			{
				return false;
			}
			glBindTexture(GL_TEXTURE_2D, m_fboTex[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (!m_fbo[i].Create())
			{
				return false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTex[i], 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		if (!m_outputTex.Create())
		{
			return false;
		}
		glBindTexture(GL_TEXTURE_2D, m_outputTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	void GLFilter::Resize(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_pingIndex = 0;

		for (int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_fboTex[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glBindTexture(GL_TEXTURE_2D, m_outputTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLFilter::Apply(GLuint inputTexture, FilterPreset preset, const FilterParams& customParams)
	{
		FilterParams params = customParams;
		if (preset != FilterPreset::None)
		{
			params = GetPresetParams(preset);
		}

		m_pingIndex = 0;

		ApplyFilter(inputTexture, m_fbo[m_pingIndex], params);
		m_pingIndex ^= 1;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFilter::ApplyFilter(GLuint inputTex, GLuint outputFbo, const FilterParams& params)
	{
		glUseProgram(m_filterProgram);

		GLint exposureLoc = glGetUniformLocation(m_filterProgram, "u_exposure");
		GLint contrastLoc = glGetUniformLocation(m_filterProgram, "u_contrast");
		GLint saturationLoc = glGetUniformLocation(m_filterProgram, "u_saturation");
		GLint hueShiftLoc = glGetUniformLocation(m_filterProgram, "u_hueShift");
		GLint temperatureLoc = glGetUniformLocation(m_filterProgram, "u_temperature");
		GLint tintLoc = glGetUniformLocation(m_filterProgram, "u_tint");

		glUniform1f(exposureLoc, params.exposure);
		glUniform1f(contrastLoc, params.contrast);
		glUniform1f(saturationLoc, params.saturation);
		glUniform1f(hueShiftLoc, params.hueShift);
		glUniform1f(temperatureLoc, params.temperature);
		glUniform1f(tintLoc, params.tint);

		glBindFramebuffer(GL_FRAMEBUFFER, outputFbo);
		glViewport(0, 0, m_width, m_height);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, inputTex);
		GLint texLoc = glGetUniformLocation(m_filterProgram, "u_texture");
		glUniform1i(texLoc, 0);

		float vertices[] = {
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			-1.0f,  1.0f,
			 1.0f,  1.0f
		};
		float uvs[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo[2];
		glGenBuffers(2, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDeleteBuffers(2, vbo);
		glDeleteVertexArrays(1, &vao);

		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	FilterParams GLFilter::GetPresetParams(FilterPreset preset)
	{
		FilterParams params;
		switch (preset)
		{
		case FilterPreset::WhiteBright:
			params.exposure = 1.3f;
			params.contrast = 0.85f;
			params.saturation = 0.7f;
			params.hueShift = 0.0f;
			params.temperature = -10.0f;
			params.tint = 5.0f;
			break;
		case FilterPreset::SkinBright:
			params.exposure = 1.15f;
			params.contrast = 0.9f;
			params.saturation = 0.85f;
			params.hueShift = 5.0f;
			params.temperature = -5.0f;
			params.tint = 3.0f;
			break;
		case FilterPreset::Dark:
			params.exposure = 0.6f;
			params.contrast = 1.3f;
			params.saturation = 0.9f;
			params.hueShift = 0.0f;
			params.temperature = 0.0f;
			params.tint = 0.0f;
			break;
		case FilterPreset::GenshinDay:
			params.exposure = 1.2f;
			params.contrast = 1.0f;
			params.saturation = 1.1f;
			params.hueShift = 10.0f;
			params.temperature = -5.0f;
			params.tint = 2.0f;
			break;
		case FilterPreset::GenshinNight:
			params.exposure = 0.7f;
			params.contrast = 1.1f;
			params.saturation = 0.8f;
			params.hueShift = -5.0f;
			params.temperature = 5.0f;
			params.tint = -2.0f;
			break;
		case FilterPreset::GenshinDusk:
			params.exposure = 1.0f;
			params.contrast = 1.05f;
			params.saturation = 1.15f;
			params.hueShift = 15.0f;
			params.temperature = 10.0f;
			params.tint = 3.0f;
			break;
		default:
			break;
		}
		return params;
	}

	std::string GLFilter::GetVertexShader() const
	{
		return
			"#version 300 es\n"
			"layout(location = 0) in vec2 a_position;\n"
			"layout(location = 1) in vec2 a_texCoord;\n"
			"out vec2 v_texCoord;\n"
			"void main()\n"
			"{\n"
			"    v_texCoord = a_texCoord;\n"
			"    gl_Position = vec4(a_position, 0.0, 1.0);\n"
			"}\n";
	}

	std::string GLFilter::GetFragmentShader() const
	{
		return
			"#version 300 es\n"
			"precision highp float;\n"
			"in vec2 v_texCoord;\n"
			"out vec4 fragColor;\n"
			"uniform sampler2D u_texture;\n"
			"uniform float u_exposure;\n"
			"uniform float u_contrast;\n"
			"uniform float u_saturation;\n"
			"uniform float u_hueShift;\n"
			"uniform float u_temperature;\n"
			"uniform float u_tint;\n"
			"vec3 rgb2hsv(vec3 c)\n"
			"{\n"
			"    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
			"    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
			"    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"
			"    float d = q.x - min(q.w, q.y);\n"
			"    float e = 1.0e-10;\n"
			"    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
			"}\n"
			"vec3 hsv2rgb(vec3 c)\n"
			"{\n"
			"    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
			"    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
			"    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
			"}\n"
			"void main()\n"
			"{\n"
			"    vec4 color = texture(u_texture, v_texCoord);\n"
			"    vec3 rgb = color.rgb;\n"
			"    rgb *= u_exposure;\n"
			"    rgb = (rgb - 0.5) * u_contrast + 0.5;\n"
			"    vec3 hsv = rgb2hsv(rgb);\n"
			"    hsv.x += u_hueShift / 360.0;\n"
			"    hsv.y *= u_saturation;\n"
			"    rgb = hsv2rgb(hsv);\n"
			"    float r = rgb.r;\n"
			"    float g = rgb.g;\n"
			"    float b = rgb.b;\n"
			"    r += u_temperature * 0.01;\n"
			"    b -= u_temperature * 0.01;\n"
			"    g += u_tint * 0.01;\n"
			"    r = clamp(r, 0.0, 1.0);\n"
			"    g = clamp(g, 0.0, 1.0);\n"
			"    b = clamp(b, 0.0, 1.0);\n"
			"    fragColor = vec4(r, g, b, color.a);\n"
			"}\n";
	}
}