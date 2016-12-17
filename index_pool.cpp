#include "index_pool.hpp"
#include <cmath>
#include <cstring>

IndexPool::IndexPool(unsigned int size)
  : size_ {size}
  , depth_ {0u}
{
	if (size == 0) {
		return;
	}

	depth_ = compute_depth(size);

	unsigned int tree_size = ((1 << (POW_N_SPLIT * depth_)) - 1) / (N_SPLIT - 1);
	unsigned int cursor_depth = depth_;
	unsigned int cursor_depth_index = size_;
	unsigned int cursor_depth_size = 1 << (POW_N_SPLIT * cursor_depth);
	unsigned int cursor_depth_begin = (cursor_depth_size - 1) / (N_SPLIT - 1);
	unsigned int cursor_depth_offset = 0;

	tree_.resize(tree_size);
	std::fill(tree_.begin(), tree_.end(), ~0u);

	while (cursor_depth_index < cursor_depth_size and cursor_depth > 0) {
		cursor_depth--;
		cursor_depth_offset = cursor_depth_index % N_SPLIT;
		cursor_depth_index /= N_SPLIT;
		cursor_depth_size /= N_SPLIT;
		cursor_depth_begin -= cursor_depth_size;
		for (unsigned int i = cursor_depth_index; i < cursor_depth_size; i++) {
			tree_[cursor_depth_begin + i] = 0u;
			if (i == cursor_depth_index) {
				tree_[cursor_depth_begin + i] += ((1 << cursor_depth_offset) - 1);
			}
		}
		if (cursor_depth_offset > 0) {
			cursor_depth_offset = 0;
			cursor_depth_index++;
		}
	}
}

bool IndexPool::has_unused_indices() const
{
	return tree_.size() > 0 and tree_[0] != 0u;
}

bool IndexPool::is_index_used(unsigned int index) const
{
	if (index >= size_) {
		return true;
	}

	unsigned int leaf_depth = depth_ - 1;
	unsigned int leaf_depth_size = 1 << (POW_N_SPLIT * leaf_depth);
	unsigned int leaf_depth_begin = (leaf_depth_size - 1) / (N_SPLIT - 1);

	return not (tree_[leaf_depth_begin + index / N_SPLIT] & (1 << (index % N_SPLIT)));
}

Optional<unsigned int> IndexPool::allocate()
{
	unsigned int cursor_depth = 0;
	unsigned int cursor_depth_index = 0;
	unsigned int cursor_depth_offset = 0;
	unsigned int cursor_depth_begin = 0;
	unsigned int cursor_depth_size = 1;

	if (not has_unused_indices()) {
		return {};
	}

	while (cursor_depth < depth_) {
		cursor_depth_offset = get_first_set_bit_offset(tree_[cursor_depth_begin + cursor_depth_index]);
		cursor_depth_begin += cursor_depth_size;
		cursor_depth_size *= N_SPLIT;
		cursor_depth_index = N_SPLIT * cursor_depth_index + cursor_depth_offset;
		cursor_depth++;
	}

	bubble_up_change(false, cursor_depth_index);
	return cursor_depth_index;
}

void IndexPool::free(unsigned int index)
{
	bubble_up_change(true, index);
}

unsigned int IndexPool::compute_depth(unsigned int size)
{
	size -= 1;
	int depth = 0;
	while (size >>= 1) {
		depth++;
	}
	return depth / POW_N_SPLIT + 1;
}

unsigned char IndexPool::get_first_set_bit_offset(unsigned int n)
{
	return ffs(n) - 1;
}

void IndexPool::bubble_up_change(bool set, unsigned int index)
{
	unsigned int cursor_depth = depth_;
	unsigned int cursor_depth_index = index;
	unsigned int cursor_depth_offset = 0;
	unsigned int cursor_depth_size = 1 << (POW_N_SPLIT * cursor_depth);
	unsigned int cursor_depth_begin = (cursor_depth_size - 1) / (N_SPLIT - 1);

	bool is_done = false;
	while (not is_done and cursor_depth_index < cursor_depth_size and cursor_depth > 0) {
		cursor_depth--;
		cursor_depth_offset = cursor_depth_index % N_SPLIT;
		cursor_depth_index /= N_SPLIT;
		cursor_depth_size /= N_SPLIT;
		cursor_depth_begin -= cursor_depth_size;

		if (set) {
			is_done = tree_[cursor_depth_begin + cursor_depth_index];
			tree_[cursor_depth_begin + cursor_depth_index] |= (1 << cursor_depth_offset);
		} else {
			tree_[cursor_depth_begin + cursor_depth_index] &= ~(1 << cursor_depth_offset);
			is_done = tree_[cursor_depth_begin + cursor_depth_index];
		}
	}
}
