#include "JsonParser.h"
#include <fstream>
#include <sstream>
#include "Material.h"

JsonParser::JsonParser() noexcept : _doc()
{
}

JsonParser::JsonParser(const std::string& path) : JsonParser()
{
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string jsonStr = buffer.str();

	file.close();

	// jsonStr = "{\"project\":\"rapidjson\",\"stars\":10}";
	_doc.Parse(jsonStr.c_str());
}

Material JsonParser::getMaterial(const std::string& location)
{
	static float inv = 1.f / 255.f;
	rapidjson::Value& obj = _doc[location.c_str()];
	assert(obj.IsObject());

	rapidjson::Value& a1 = obj["albedo1"];
	assert(a1.IsObject());
	bool isNormalised = a1["normalised"].GetBool();
	float r = a1["r"].GetFloat();
	float g = a1["g"].GetFloat();
	float b = a1["b"].GetFloat();
	float a = a1["a"].GetFloat();
	glm::vec4 albedo1(r, g, b, a);
	if (!isNormalised) albedo1 *= inv;

	rapidjson::Value& a2 = obj["albedo2"];
	assert(a2.IsObject());
	isNormalised = a1["normalised"].GetBool();
	r = a2["r"].GetFloat();
	g = a2["g"].GetFloat();
	b = a2["b"].GetFloat();
	a = a2["a"].GetFloat();
	glm::vec4 albedo2(r, g, b, a);
	if (!isNormalised) albedo2 *= inv;

	Material res(albedo1, albedo2);
	return res;
}

std::vector<Material> JsonParser::getAllMaterial()
{
	std::vector<Material> res;
	for (auto& obj : _doc.GetObject()) {
		res.push_back(getMaterial(obj.name.GetString()));
	}
	return res;
}
