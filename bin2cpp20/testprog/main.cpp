#include "test.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {

	if(argc == 1) {
		for(const auto &i : value1_arr) {
			std::cout << i;
		}
	} else if(argc == 2) {
		std::fstream file;
		file.open(argv[1], std::ios::out | std::ios::binary);
		file.write(reinterpret_cast<const char *>(value1_arr.data()), value1_arr.size());
		file.close();
	}
	return 0;
}