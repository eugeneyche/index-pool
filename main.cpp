#include "index_pool.hpp"
#include <iostream>
#include <memory>
#include <sstream>

bool read_line_into_tokens(std::vector<std::string>& tokens)
{
	std::string line;
	std::getline(std::cin, line);
	std::stringstream tokenizer (line);
	std::string token;
	tokens.clear();
	while (tokenizer >> token) {
		tokens.push_back(token);
	}
	return not tokens.empty();
}

int main()
{
	std::vector<std::string> tokens;
	std::unique_ptr<IndexPool> pool;
	while (read_line_into_tokens(tokens)) {
		if (tokens[0] == "new") {
			pool = std::make_unique<IndexPool>(std::stoi(tokens[1]));
		} else if (tokens[0] == "alloc") {
			Optional<unsigned int> index = pool->allocate();
			if (index.has_value()) {
				std::cout << index.unwrap() << std::endl;
			} else {
				std::cout << "No index." << std::endl;
			}
		} else if (tokens[0] == "free") {
			pool->free(std::stoi(tokens[1]));
		} else if (tokens[0] == "check") {
			if (pool->is_index_used(std::stoi(tokens[1]))) {
				std::cout << "Used." << std::endl;
			} else {
				std::cout << "Unused." << std::endl;
			}
		} else {
			std::cout << "Invalid command." << std::endl;
		}
	}
}
