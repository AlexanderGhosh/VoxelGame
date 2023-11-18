#pragma once
#include <string>

#include <glm.hpp>

class Shader {
public:
	Shader();
	Shader(std::string shaderName, bool hasGeom = false);
	unsigned int getId() const;
	void setUp();
	void setName(const std::string& name);
	void bind() const;
	void unBind() const;
	bool setValue(const std::string& name, const glm::mat4& value) const;
	bool setValue(const std::string& name, const glm::vec4& value) const;
	bool setValue(const std::string& name, const glm::vec3& value) const;
	bool setValue(const std::string& name, const glm::vec2& value) const;
	bool setValue(const std::string& name, const float& value) const;
	bool setValue(const std::string& name, const int& value) const;
private:
	bool hasGeom;
	unsigned int program;
	std::string name;

	const int getLocation(const std::string& name) const;
	void setLocation(const int& location, const glm::mat4& value) const;
	void setLocation(const int& location, const glm::vec4& value) const;
	void setLocation(const int& location, const glm::vec3& value) const;
	void setLocation(const int& location, const glm::vec2& value) const;
	void setLocation(const int& location, const float& value) const;
	void setLocation(const int& location, const int& value) const;
};