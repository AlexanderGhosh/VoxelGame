#include "Game/Player/Camera.h"
#include "Buffer.h"

class Ray
{
public:
	Ray();
	Ray(glm::vec3 origin, glm::vec3 dir, float max, bool visable = 0);
	glm::vec3& getDirection();
	glm::vec3& getOrigin();


	void setVisable(const bool visable);
	void setMax(const float max);
	void setOrigin(const glm::vec3 origin);
	void setDirection(const glm::vec3 dir);
	void render(Camera cam, glm::mat4 projection, glm::vec3 colour = glm::vec3(1, 0, 0));
	void translate(glm::vec3 translation);
	float checkIntercesction_Block(glm::vec3 blockPos_abs, FACES_NAMES face);
private:
	float maxLen;
	bool visable;
	glm::vec3 origin, end, direction;
	unsigned int VAO;
	void setUpRender();
	void calcEnd();
};

