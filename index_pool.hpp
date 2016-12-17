#pragma once
#include "optional.hpp"
#include <vector>

class IndexPool
{
public:
	IndexPool(unsigned int size);
	virtual ~IndexPool() = default;

	bool has_unused_indices() const;
	bool is_index_used(unsigned int index) const;
	Optional<unsigned int> allocate();
	void free(unsigned int index);

private:
	static const unsigned int N_SPLIT = 32;
	static const unsigned int POW_N_SPLIT = 5;

	unsigned int compute_depth(unsigned int size);
	unsigned char get_first_set_bit_offset(unsigned int n);
	void bubble_up_change(bool set, unsigned int index);

	unsigned int size_;
	unsigned int depth_;
	std::vector<unsigned int> tree_;	
};
