#include "test.hpp"
#include "test2.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale());
	if(argc == 1) {
		for(int i = 0; i < var2_length; ++i) {
			std::wcout << L"->" << var2[i] << L"\n";
		}
	} else if(argc == 2) {
		std::fstream file;
		file.open(argv[1], std::ios::out | std::ios::binary);
		file.write(reinterpret_cast<const char *>(bin_arr.data()), bin_arr.size());
		file.close();
	}
	return 0;
}
