#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Glyph.h"
#include "../Float4.h"

namespace GUI::Utils::Text {
	class GlyphRendering {
	public:
		using Font = std::vector<Glyph>;
		static GlyphRendering& getInstance() {
			static GlyphRendering instance;
			return instance;
		}

		void loadFont(const std::string& fontName, const std::string& fileLocation);
		// renders all the text in text as one line
		// position = bottom left of the line (as if you were writing it) (in pixel coords)
		void drawSentence(const std::string& text, Float2 position, float scale, const Float4& colour = {}, const std::string& font = "") const;
		Float2 getSentenceLength(const std::string& text, float scale, const std::string& font = "") const;


		// vertex data is 4 float4 drawn with triangle strips (pixel coords, texture coords)
		void setShader(unsigned int id);

		~GlyphRendering();
	private:
		GlyphRendering();

		GlyphRendering(const GlyphRendering&) = delete;
		GlyphRendering& operator=(const GlyphRendering&) = delete;

		GlyphRendering(GlyphRendering&&) = delete;
		GlyphRendering& operator=(GlyphRendering&&) = delete;


		std::unordered_map<std::string, Font> _fonts;
		unsigned int VBO, VAO;
		unsigned int _shaderID;
		std::string _defaultFont;
	};
}