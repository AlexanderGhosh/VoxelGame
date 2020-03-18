#include "Ray.h"
Ray::Ray() : maxLen(-1), origin(0), direction(0), visable(0), end(0), VAO(0) { }
Ray::Ray(glm::vec3 origin, glm::vec3 dir, GLfloat max, GLboolean visable) : origin(origin), direction(dir), maxLen(max), visable(visable), end(0), VAO(0) {
	calcEnd();
}
void Ray::setMax(const GLfloat max) {
	maxLen = max;
}
void Ray::setOrigin(const glm::vec3 origin) {
	this->origin = origin;
	calcEnd();
}
void Ray::setDirection(const glm::vec3 dir) {
	direction = dir;
	calcEnd();
}
void Ray::setVisable(const GLboolean visable) {
	this->visable = visable;
}
void Ray::setUpRender() {
	GLuint VBO;
	GLfloat points[] = {
		origin.x, origin.y, origin.z,
		end.x, end.y, end.z
	};
	if (VAO) glDeleteVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
}
void Ray::render(Camera cam, glm::mat4 projection, glm::vec3 colour) {
	if (!visable) return;
	auto& shader = SHADERS[RAY];
	glm::mat4 view(1);
	view = cam.GetViewMatrix();

	shader->bind();
	shader->setValue("projection", projection);
	shader->setValue("view", view);
	shader->setValue("colour", colour);
	glBindVertexArray(VAO);
	glLineWidth(100);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
	shader->unBind();
}
void Ray::translate(glm::vec3 translation) {
	origin += translation;
	calcEnd();
}
void Ray::calcEnd() {
	end = origin + direction * maxLen;
	if (visable) setUpRender();
}
GLfloat sumVec(glm::vec3 vec) {
	return vec.x + vec.y + vec.z;
}
GLfloat Ray::checkIntercesction_Block(glm::vec3 blockPos_abs, FACES_NAMES face) {
	std::map<FACES_NAMES, glm::vec3> normals = {
		{ TOP, {0, 1, 0} },
		{ BOTTOM, {0, 1, 0} },
		{ FRONT, {0, 0, 1} },
		{ BACK, {0, 0, -1} },
		{ LEFT, {1, 0, 0} },
		{ RIGHT, {1, 0, 0} }
	};

	glm::vec3 max(blockPos_abs + glm::vec3(0.5f));
	glm::vec3 min(blockPos_abs - glm::vec3(0.5f));
	glm::vec3 normal = normals[face];
	auto vertices = FACES[face]->getVertices(1, 0);
	glm::vec3 vertex = vertices[0] + blockPos_abs;
	/*direction = { 0, 0, 1 };
	blockPos_abs = { 0, -10, 3 };
	normal = { 0, 0, -1 };
	origin = { 0, -9.5, 0 };
	glm::vec3 vertex = t1[0] + blockPos_abs;*/

	GLfloat k = glm::dot(vertex, normal); // equation of plane

	GLfloat coefficant = glm::dot(normal, direction);
	if (coefficant == 0) return -1; // no intersection
	GLfloat neumirator = k - glm::dot(normal, origin);
	GLfloat t  = neumirator / coefficant;

	glm::vec3 point = origin + t * (direction);

	if (!glm::all(glm::lessThanEqual(point, max) && glm::greaterThanEqual(point, min))) return -1;

	GLfloat distance = std::abs(glm::distance(origin, point));
	return distance;
}