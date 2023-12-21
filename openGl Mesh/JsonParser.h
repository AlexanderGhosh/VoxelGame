#pragma once
#include <string>
#include <vector>
#include <document.h>

class Material;
class JsonParser
{
public:
	JsonParser() noexcept;
	JsonParser(const std::string& path);

	std::vector<Material> getAllMaterial();
private:
	Material getMaterial(const std::string& location);
	rapidjson::Document _doc;
};
