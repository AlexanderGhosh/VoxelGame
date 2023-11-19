#include "GizmoManager.h"
#include "../Gizmos/IShape.h"

GizmoManager::GizmoManager() : allGizmos()
{
}

void GizmoManager::addGizmo(Gizmo::IShape* gizmo)
{
	allGizmos.push_back(gizmo);
}

void GizmoManager::addGizmo(Gizmo::IShape& gizmo)
{
	addGizmo(&gizmo);
}

void GizmoManager::render(const glm::mat4& pv) const
{
	for (Gizmo::IShape* gizmo : allGizmos) {
		gizmo->draw(pv);
	}
}

void GizmoManager::destroy()
{
	for (Gizmo::IShape* gizmo : allGizmos) {
		gizmo->cleanUp();
	}
}
