#include "Game/Player/Camera.h"

class Ray
{
public:
	Ray();
	Ray(glm::vec3 origin, glm::vec3 dir, GLfloat max, GLboolean visable = 0);
	void setVisable(const GLboolean visable);
	void setMax(const GLfloat max);
	void setOrigin(const glm::vec3 origin);
	void setDirection(const glm::vec3 dir);
	void render(Camera cam, glm::mat4 projection, glm::vec3 colour = glm::vec3(1, 0, 0));
	void translate(glm::vec3 translation);
	FACES_NAMES checkIntercesction_Block(glm::vec3 blockPos_abs);
private:
	GLfloat maxLen;
	GLboolean visable;
	glm::vec3 origin, end, direction;
	GLuint VAO;
	void setUpRender();
	void calcEnd();
};

