#pragma once
#include <vector>

enum class Block : unsigned char;

class BlocksEncoded {
public:
	BlocksEncoded();

	void push(const Block block, const unsigned int count);

	const unsigned int height() const;
	const size_t size() const;

	const Block block(const unsigned int i) const;
	const unsigned int count(const unsigned int i) const;

	const Block operator[] (const int height) const;
private:
	std::vector<Block> blocks_;
	std::vector<unsigned int> counts_;
	unsigned int height_;
};
