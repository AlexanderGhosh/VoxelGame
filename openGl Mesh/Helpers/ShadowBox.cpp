#include "ShadowBox.h"
#include <gtx/string_cast.hpp>
#include "Functions.h"
#include "../Game/Player/Camera.h"
#include "Constants.h"
#include "Functions.h"

std::vector<glm::vec3> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
	std::vector<glm::vec3> frustrumCorners = {
		glm::vec3(-1,  1, -1),
		glm::vec3( 1,  1, -1), // maybe z = 0
		glm::vec3( 1, -1, -1),
		glm::vec3(-1, -1, -1),

		glm::vec3(-1,  1, 1),
		glm::vec3( 1,  1, 1),
		glm::vec3( 1, -1, 1),
		glm::vec3(-1, -1, 1)
	};

	glm::mat4 pvInv = glm::inverse(proj * view);
	for (glm::vec3& c : frustrumCorners) {
		auto t = pvInv * glm::vec4(c, 1);
		c = glm::vec3(t) / t.w;
	}
	return frustrumCorners;
}

glm::mat4 ShadowBox::getLSM(Camera& camera, const glm::mat4& proj, const glm::vec3& lightPos)
{
	auto corners = getFrustumCornersWorldSpace(proj, camera.GetViewMatrix());
	glm::vec3 frustrumCenter(0);
	for (const glm::vec3& c : corners) {
		frustrumCenter += c;
	}
	frustrumCenter /= 8.f;

	float radius = glm::distance(corners[3], corners[5]) * .5f;
	float texelsPerUnit = SHADOW_MAP_SIZE / (radius * 2.f);

	glm::mat4 scalar(texelsPerUnit);
	//scalar[3][3] = 1;
	glm::vec3 lightDir = glm::normalize(-lightPos);
	glm::mat4 lookat = glm::lookAt(glm::vec3(0), lightDir, glm::vec3(0, 1, 0));
	lookat *= scalar;
	glm::mat4 lookatInv = glm::inverse(lookat);

	glm::vec4 t = lookat * glm::vec4(frustrumCenter, 1);
	frustrumCenter = glm::vec3(t) / t.w;
	frustrumCenter.x = floorf(frustrumCenter.x);
	frustrumCenter.z = floorf(frustrumCenter.z);
	t = lookatInv * glm::vec4(frustrumCenter, 1);
	frustrumCenter = glm::vec3(t) / t.w;

	glm::vec3 eye = frustrumCenter - (lightDir * radius * 2.f);

	glm::mat4 lightView = glm::lookAt(eye, frustrumCenter, glm::vec3(0, 1, 0));

	float multi = 6;
	glm::mat4 lightProj = glm::ortho(-radius, radius, -radius, radius, -radius * multi, radius * multi);

	return lightProj * lightView;
}

