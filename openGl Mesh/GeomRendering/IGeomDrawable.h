#pragma once
struct DrawData;

class IGeomDrawable {
public:
	virtual DrawData getDrawData() const = 0;
};

