#include "GlyphRendering.h"
#include <freetype/freetype.h>
#include <glad/glad.h>

using namespace GUI::Utils::Text;

GlyphRendering::GlyphRendering() : _fonts(), VBO(), VAO(), _defaultFont(), _shaderID(0) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void GUI::Utils::Text::GlyphRendering::loadFont(const std::string& fontName, const std::string& fileLocation)
{
	if (_fonts.contains(fontName)) return;
	if (_defaultFont.empty()) _defaultFont = fontName;

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		throw "ERROR::FREETYPE: Could not init FreeType Library";

	FT_Face face;
	if (FT_New_Face(ft, fileLocation.c_str(), 0, &face))
		throw"ERROR::FREETYPE: Failed to load font";

	FT_Set_Pixel_Sizes(face, 0, 48); // font size

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) // set active glyph
		throw "ERROR::FREETYTPE: Failed to load Glyph";

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	_fonts[fontName] = Font(128);
	Font& font = _fonts.at(fontName);

	for (GLubyte c = 0; c < font.size(); c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			throw "ERROR::FREETYTPE: Failed to load Glyph";
		}

		Glyph glyph;

		glyph.size.x = face->glyph->bitmap.width;
		glyph.size.y = face->glyph->bitmap.rows;

		glyph.bearing.x = face->glyph->bitmap_left;
		glyph.bearing.y = face->glyph->bitmap_top;

		glyph.advance.x = face->glyph->advance.x;
		glyph.advance.y = face->glyph->advance.y;

		// Generate texture
		glGenTextures(1, &glyph.textureID);
		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.size.x, glyph.size.y, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use

		font[c] = glyph;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void GUI::Utils::Text::GlyphRendering::drawSentence(const std::string& text, Float2 position, float scale, const Float3& colour, const std::string& fontName)
{
	Font* font;
	if (fontName.empty()) {
		font = &_fonts.at(_defaultFont);
	}
	else {
		font = &_fonts.at(fontName);
	}
	glUseProgram(_shaderID);
	glBindVertexArray(VAO);

	unsigned int loc = glGetUniformLocation(_shaderID, "colour");
	glUniform4f(loc, colour.r, colour.g, colour.b, 1.f);


	loc = glGetUniformLocation(_shaderID, "text");
	glUniform1i(loc, 0);

	for (char c : text) {
		if (c > font->size()) {
			// if the char is not in the font then default to char = @
			c = '@';
		}
		const Glyph& g = font->at(c);

		float xpos = position.x + g.bearing.x * scale;
		float ypos = position.y - (g.size.y - g.bearing.y) * scale;

		float w = g.size.x * scale;
		float h = g.size.y * scale;

		float vertexData[16]= {
			xpos,     ypos + h,   0.0, 0.0,
			xpos,     ypos,       0.0, 1.0,
			xpos + w, ypos + h,   1.0, 0.0,
			xpos + w, ypos,       1.0, 1.0
		};

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, &vertexData);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, g.textureID);
		glActiveTexture(GL_TEXTURE0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		auto e = glGetError();



		position.x += ((int)g.advance.x >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void GUI::Utils::Text::GlyphRendering::setShader(unsigned int id)
{
	_shaderID = id;
}

GUI::Utils::Text::GlyphRendering::~GlyphRendering()
{
	for (auto& [_, font] : _fonts) {
		for (const Glyph& g : font) {
			glDeleteTextures(1, &g.textureID);
		}
	}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
