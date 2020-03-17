#include "Ray.h"
Ray::Ray() : maxLen(-1), origin(0), direction(0), visable(0), end(0), VAO(0) { }
Ray::Ray(glm::vec3 origin, glm::vec3 dir, GLfloat max, GLboolean visable) : origin(origin), direction(dir),  maxLen(max), visable(visable), end(0), VAO(0) {
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
	if(VAO) glDeleteVertexArrays(1, &VAO);
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
	if(visable) setUpRender();
}
FACES_NAMES Ray::checkIntercesction_Block(glm::vec3 blockPos_abs) {
	std::map<FACES_NAMES, glm::vec3> normals = {
		{ TOP, {0, 1, 0} },
		{ BOTTOM, {0, -1, 0} },
		{ FRONT, {0, 0, 1} },
		{ BACK, {0, 0, -1} },
		{ LEFT, {-1, 0, 0} },
		{ RIGHT, {1, -1, 0} }
	};
	std::map<FACES_NAMES, GLfloat> distances;
	for (auto& normal : normals) {
		auto& face = normal.first;
		auto& norm = normal.second;
		auto vertices = FACES[face]->getVertices(0, 0);
		GLfloat k = glm::dot(vertices[0], norm);
		GLfloat c = glm::dot(direction, norm);
		if (c == k) {
			glm::vec3 intercection;
			GLfloat t = 0;
			glm::vec3 grad = origin - end;
			grad *= norm;
			glm::vec3 i = origin * norm;
			t = (k - (i.x + i.y + i.z)) / (grad.x + grad.y + grad.z);
			distances.insert({ face, glm::distance(origin, t * (grad / norm) + origin) });
		}
	}
	GLfloat dist_ = 1000;
	FACES_NAMES face_ = NULL_;
	for (auto& dist : distances) {
		auto& face = dist.first;
		auto& dis = dist.second;
		if (abs(dis) < dist_) {
			dist_ = dis;
			face_ = face;
		}
	}
	return face_;
}