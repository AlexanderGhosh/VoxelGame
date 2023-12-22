#include "JsonParser.h"
#include <fstream>
#include <sstream>
#include "Material.h"
#include "Block.h"

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

Block JsonParser::getBlock(const std::string& location)
{
	rapidjson::Value& obj = _doc[location.c_str()];
	assert(obj.IsObject());

	bool isTransparent = obj["isTransparent"].GetBool();
	bool isDynamic = obj["isDynamic"].GetBool();
	unsigned int materialIndex = obj["materialIndex"].GetUint();

	Block res;
	res.isTransparent = isTransparent;
	res.isDynamic = isDynamic;
	res.materialIndex = materialIndex;
	return res;
}

std::vector<Material> JsonParser::getAllMaterials()
{
	std::vector<Material> res;
	for (auto& obj : _doc.GetObject()) {
		res.push_back(getMaterial(obj.name.GetString()));
	}
	return res;
}

std::vector<Block> JsonParser::getAllBlocks()
{
	std::vector<Block> res;
	for (auto& obj : _doc.GetObject()) {
		res.push_back(getBlock(obj.name.GetString()));
	}
	return res;
}
