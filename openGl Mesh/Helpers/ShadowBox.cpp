#include "ShadowBox.h"
#include "Functions.h"
#include "../Game/Player/Camera.h"


ShadowBox::ShadowBox() : lightViewMatrix(), OFFSET(10), minX(), minY(), minZ(), maxX(), maxY(), maxZ(), farWidth(), farHeight(), nearWidth(), nearHeight() { }

ShadowBox::ShadowBox(glm::mat4 lightViewMatrix) : ShadowBox() {
	this->lightViewMatrix = lightViewMatrix;
	calculateWidthsAndHeights();
}

void ShadowBox::update(Camera& camera) {
	glm::vec3 forwardVector = camera.GetFront();

	glm::vec3 toFar = forwardVector * SHADOW_DISTANCE;
	glm::vec3 toNear = forwardVector * NEAR_PLANE;
	glm::vec3 centerNear = toNear + camera.GetPosition();
	glm::vec3 centerFar = toFar + camera.GetPosition();

	std::array<glm::vec4, 8> points = calculateFrustumVertices(forwardVector, centerNear, centerFar);

	bool first = true;
	for (glm::vec4 point : points) {
		if (first) {
			minX = point.x;
			maxX = point.x;
			minY = point.y;
			maxY = point.y;
			minZ = point.z;
			maxZ = point.z;
			first = false;
			continue;
		}
		if (point.x > maxX) {
			maxX = point.x;
		}
		else if (point.x < minX) {
			minX = point.x;
		}
		if (point.y > maxY) {
			maxY = point.y;
		}
		else if (point.y < minY) {
			minY = point.y;
		}
		if (point.z > maxZ) {
			maxZ = point.z;
		}
		else if (point.z < minZ) {
			minZ = point.z;
		}
	}
	maxZ += OFFSET;

}

const glm::vec3 ShadowBox::getCenter()  const {
	float x = (minX + maxX) * 0.5f;
	float y = (minY + maxY) * 0.5f;
	float z = (minZ + maxZ) * 0.5f;
	glm::vec4 cen(x, y, z, 1);
	glm::mat4 invertedLight = glm::inverse(lightViewMatrix);
	return glm::vec3(invertedLight * cen);
}

const float ShadowBox::getWidth()  const {
	return maxX - minX;
}


const float ShadowBox::getHeight()  const {
	return maxY - minY;
}

const float ShadowBox::getLength()  const {
	return maxZ - minZ;
}

std::array<glm::vec4, 8> ShadowBox::calculateFrustumVertices(glm::vec3 forwardVector, glm::vec3 centerNear, glm::vec3 centerFar) {
	glm::vec3 upVector(0, 1, 0);
	glm::vec3 rightVector = glm::cross(forwardVector, upVector);
	glm::vec3 downVector = -upVector;
	glm::vec3 leftVector = -rightVector;

	glm::vec3 farTop = centerFar + upVector * farHeight;
	glm::vec3 farBottom = centerFar + downVector * farHeight;

	glm::vec3 nearTop = centerNear + upVector * nearHeight;
	glm::vec3 nearBottom = centerNear + downVector * nearHeight;

	std::array<glm::vec4, 8> points{};
	points[0] = calculateLightSpaceFrustumCorner(farTop, rightVector, farWidth);
	points[1] = calculateLightSpaceFrustumCorner(farTop, leftVector, farWidth);
	points[2] = calculateLightSpaceFrustumCorner(farBottom, rightVector, farWidth);
	points[3] = calculateLightSpaceFrustumCorner(farBottom, leftVector, farWidth);
	points[4] = calculateLightSpaceFrustumCorner(nearTop, rightVector, nearWidth);
	points[5] = calculateLightSpaceFrustumCorner(nearTop, leftVector, nearWidth);
	points[6] = calculateLightSpaceFrustumCorner(nearBottom, rightVector, nearWidth);
	points[7] = calculateLightSpaceFrustumCorner(nearBottom, leftVector, nearWidth);
	return points;
}

glm::vec4 ShadowBox::calculateLightSpaceFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width) {
	glm::vec3 point = startPoint + direction * width;
	glm::vec4 point4f = glm::vec4(point.x, point.y, point.z, 1);
	return lightViewMatrix * point4f;
}

void ShadowBox::calculateWidthsAndHeights() {
	farWidth = SHADOW_DISTANCE * tanf(radians(FOV));
	nearWidth = NEAR_PLANE * tanf(radians(FOV));
	farHeight = farWidth / ASPECT_RATIO;
	nearHeight = nearWidth / ASPECT_RATIO;
}

glm::mat4 ShadowBox::getProjection() const
{
	glm::mat4 res(1);
	res[0][0] = 2 / getWidth();
	res[1][1] = 2 / getHeight();
	res[2][2] = 2 / getLength();

	res[3][0] = -(maxX + minX) / getWidth();
	res[3][1] = -(maxY + minY) / getHeight();
	res[3][2] = -(maxZ + minZ) / getLength();
	return res;
}

