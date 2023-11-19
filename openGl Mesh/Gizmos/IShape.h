#pragma once
#include <glm.hpp>

class Shader;
namespace Gizmo {
	// All types of gizmos
	enum class Types : unsigned int {
		Point, Line, Circle, Cuboide, Sphere, Triangle, Arrow, Grid2D
	};
	// Interface for a Gizmo shape does its nothing by itself
	class IShape {
	protected:
		glm::vec3 position, colour;
		float thickness;
		static unsigned int pointVAO, VBO;
		Shader* shader;

		IShape();
		~IShape();
		void bindShader(const glm::mat4& pv) const;
		void bindVAO() const;
		void unbindVAO() const;
		void drawGeneral();
	public:
		virtual void draw(const glm::mat4& pv) = 0;
		virtual void cleanUp() = 0;
		virtual Types getType() const = 0;

		void setThickness(const unsigned int size);
		const glm::vec3& getPosition() const;
		const glm::vec3& getColour() const;

		virtual void setPosition(const glm::vec3& pos);
		virtual void setColour(const glm::vec3& colour);
	};
}