//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_TEXTUREUTIL_H_
#define SABA_GL_TEXTUREUTIL_H_

#include "GLObject.h"

#include <string>
#include <vector>

namespace saba
{
	GLTextureObject CreateTextureFromFile(const char* filename, bool genMipMap = true, bool rgba = false);
	GLTextureObject CreateTextureFromFile(const std::string& filename, bool genMipMap = true, bool rgba = false);

	bool LoadTextureFromFile(const GLTextureObject& tex, const char* filename, bool genMipMap = true, bool rgba = false);
	bool LoadTextureFromFile(const GLTextureObject& tex, const std::string& filename, bool genMipMap = true, bool rgba = false);

	bool IsAlphaTexture(GLuint tex);

	enum class TextureCompressFormat
	{
		None,
		ETC2_RGB,
		ETC2_RGBA,
	};

	GLTextureObject LoadTextureCompressed(const char* filename, TextureCompressFormat compressFormat = TextureCompressFormat::None, bool genMipMap = true);
	GLTextureObject LoadTextureCompressed(const std::string& filename, TextureCompressFormat compressFormat = TextureCompressFormat::None, bool genMipMap = true);

	bool ConvertToETC2(const std::vector<uint8_t>& sourceData, int width, int height, int numChannels,
		std::vector<uint8_t>* outData, TextureCompressFormat format);
}

#endif // !SABA_GL_TEXTUREUTIL_H_