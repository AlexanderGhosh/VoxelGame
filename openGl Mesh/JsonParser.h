#pragma once
#include <string>
#include <vector>
#include <document.h>

class Material;
class Block;
class JsonParser
{
public:
	JsonParser() noexcept;
	JsonParser(const std::string& path);

	std::vector<Material> getAllMaterials();
	std::vector<Block> getAllBlocks();
private:
	Material getMaterial(const std::string& location);
	Block getBlock(const std::string& location);
	rapidjson::Document _doc;
};
