#pragma once
#include "Manager.h"
#include <glm.hpp>
#include <list>


namespace Gizmo {
	class IShape;
}
class GizmoManager : public Manager<GizmoManager>
{
private:
	std::list<Gizmo::IShape*> allGizmos;
public:
	GizmoManager();
	void addGizmo(Gizmo::IShape* gizmo);
	void addGizmo(Gizmo::IShape& gizmo);
	void render(const glm::mat4& pv) const;
	void destroy() override;
};

