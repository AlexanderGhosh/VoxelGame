#pragma once
#include <vector>
#include "../Block.h"

class BlocksEncoded {
public:
	BlocksEncoded();

	void push(const Block block, const unsigned int count);

	const unsigned int height() const;
	const size_t size() const;

	const Block block(const unsigned int i) const;
	const unsigned int count(const unsigned int i) const;

	const Block operator[] (const int height) const;

	const unsigned int getUnalteredHeight() const;
	void setUnalteredHeight(const unsigned int height);
private:
	std::vector<Block> blocks_;
	std::vector<unsigned int> counts_;
	unsigned int height_;
	unsigned int unalteredHeight_;
};
