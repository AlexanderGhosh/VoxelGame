#pragma once
#include <string>

#include <glm.hpp>

// copy constructor doesnt copy program
class Shader {
public:
	Shader();
	Shader(std::string shaderName, bool hasGeom = false);
	// doesnt need extension
	Shader(const std::string& vert, const std::string& frag);
	// doesnt need extension
	Shader(const std::string& vert, const std::string& geom, const std::string& frag);
	~Shader();

	Shader(const Shader& other);
	Shader& operator=(const Shader& other);

	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	unsigned int getId() const;

	void setUp();

	void bind() const;

	void unBind() const;

	bool setValue(const std::string& name, const glm::mat4& value) const;
	bool setValue(const std::string& name, const glm::vec4& value) const;
	bool setValue(const std::string& name, const glm::vec3& value) const;
	bool setValue(const std::string& name, const glm::vec2& value) const;
	bool setValue(const std::string& name, const float& value) const;
	bool setValue(const std::string& name, const int& value) const;
	bool setValueUBO(const std::string& name, const int& location) const;
private:
	unsigned int program_;
	std::string vert_, geom_, frag_;

	const int getLocation(const std::string& name) const;
	void setLocation(const int& location, const glm::mat4& value) const;
	void setLocation(const int& location, const glm::vec4& value) const;
	void setLocation(const int& location, const glm::vec3& value) const;
	void setLocation(const int& location, const glm::vec2& value) const;
	void setLocation(const int& location, const float& value) const;
	void setLocation(const int& location, const int& value) const;
};