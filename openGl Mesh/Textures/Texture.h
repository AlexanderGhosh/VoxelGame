#pragma once
#include <glm.hpp>
#include <string>
#include "../SOIL2/SOIL2.h"

class Texture
{
public:
	Texture();
	Texture(std::string name, std::string overload);
	Texture(bool loadTex);
	Texture(std::string name);
	~Texture();

	/*Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;*/

	void bind() const;
	const unsigned int& getTexMap() const;
	const std::string& getName() const;
	const bool get2D() const;
	const glm::ivec2& getDimentions() const;
	void unBind() const;
	bool load3D(const std::string& name);
	bool load2D(const std::string& name);
private:
	bool _created;
	glm::ivec2 _dimentions;
	unsigned int _texMap;
	bool _is2D;
	std::string _name;
};

