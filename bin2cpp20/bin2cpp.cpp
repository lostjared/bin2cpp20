/* 

Compile with GCC 13.2 or greater 
bin2cpp20 - conversion tool

*/

#include "argz.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

void convertStreamToVector(std::string_view name, std::istream &in, std::ostream &out);
void convertStreamToArray(std::string_view name, const char *data, std::size_t length, std::ostream &out);
void convertStreamToString(bool sorted, std::string_view name, std::istream &in, std::ostream &out);
void stringOutputVector(const std::vector<unsigned char> &v);
template <std::size_t N>
void stringOutputArray(std::array<unsigned char, N> &a);
bool is_Valid(const std::string &name);
std::string convertString(const std::string &text);

int main(int argc, char **argv) {
	if(argc == 1) {
		convertStreamToVector("bin_vec", std::cin, std::cout);
		return EXIT_SUCCESS;
	}
	try {
		Argz<std::string> argz(argc, argv);
		argz.addOptionSingleValue('i', "input file/stdin").addOptionDoubleValue('I', "input", "input file/stdin").addOptionSingleValue('o', "output").addOptionDoubleValue('O', "output", "output file").addOptionSingle('h', "help").addOptionDouble('H', "help", "help message").addOptionSingleValue('v', "variable name").addOptionDoubleValue('V', "variable", "variable name").addOptionSingle('s', "string output").addOptionDouble('S', "string", "string output").addOptionSingle('z', "sort").addOptionDouble('Z', "sort", "sort string");
		Argument<std::string> arg;
		int value{};
		std::string input_file, output_file, variable_name;
		bool as_string = false;
		bool sorted = false;
		while((value = argz.proc(arg)) != -1) {
			switch(value) {
			case 'i':
			case 'I':
				input_file = arg.arg_value;
				break;
			case 'o':
			case 'O':
				output_file = arg.arg_value;
				break;
			case 'H':
			case 'h':
				argz.help(std::cout);
				return EXIT_SUCCESS;
			case 'v':
			case 'V':
				variable_name = arg.arg_value;
				break;
			case 's':
			case 'S':
				as_string = true;
				break;
			case 'z':
			case 'Z':
				sorted = true;
				break;
			}
		}
		if(input_file.length() == 0) {
			std::cerr << "Error input file not specified...\n";
			argz.help(std::cout);
			return EXIT_SUCCESS;
		} else {
			if(variable_name.length() == 0) {
				std::cerr << "Requires variable name... use -v\n";
				argz.help(std::cout);
				return EXIT_FAILURE;
			}
			if(!is_Valid(variable_name)) {
				std::cerr << "Error invalid variable name..\n";
				return EXIT_FAILURE;
			}

			if(input_file == "stdin") {
				convertStreamToString(sorted, variable_name, std::cin, std::cout);
				return EXIT_SUCCESS;
			}
			std::fstream file;
			file.open(input_file, std::ios::in | std::ios::binary | std::ios::ate);
			if(!file.is_open()) {
				std::cerr << "Could not open file: " << input_file << "\n";
				return EXIT_FAILURE;
			}
			std::streamsize len{file.tellg()};
			file.seekg(0, std::ios::beg);
			std::unique_ptr<char[]> buf{new char[len + 1]};
			file.read(buf.get(), len);
			file.close();
			if(output_file.length() == 0) {
				if(as_string == false) {
					variable_name = "bin_" + variable_name;
					convertStreamToArray(variable_name + "_arr", buf.get(), len, std::cout);
				} else {
					variable_name = "str_" + variable_name;
					std::istringstream in(buf.get());
					convertStreamToString(sorted, variable_name, in, std::cout);
				}
				return EXIT_SUCCESS;
			} else {
				std::fstream file;
				const auto pos{output_file.rfind(".hpp")};
				if(pos == std::string::npos)
					output_file += ".hpp";

				file.open(output_file, std::ios::out);
				if(!file.is_open()) {
					std::cerr << "Error could not open output file..\n";
					return EXIT_FAILURE;
				}
				file << "#ifndef __ARR_H_HPP_" << variable_name << "\n";
				file << "#define __ARR_H_HPP_" << variable_name << "\n";
				if(as_string == false) {
					file << "#include<array>\n\n";
					variable_name = "bin_" + variable_name;
					convertStreamToArray(variable_name + "_arr", buf.get(), len, file);
				} else {
					file << "#include<string>\n\n";
					variable_name = "str_" + variable_name;
					std::istringstream in(buf.get());
					convertStreamToString(sorted, variable_name, in, file);
				}
				file << "\n\n#endif\n";
				file.close();
			}
		}
	} catch(const ArgException<std::string> &e) {
		std::cerr << "Syntax Error: " << e.text() << "\n";
	}

	return 0;
}

void convertStreamToVector(std::string_view name, std::istream &in, std::ostream &out) {
	out << "inline const std::vector<unsigned char> " << name << " {";
	std::size_t index = 0;

	while(!in.eof()) {
		uint8_t c{};
		in.read(reinterpret_cast<char *>(&c), sizeof(uint8_t));
		const std::string hex{std::format("0x{:X}", static_cast<uint8_t>(c))};
		if(in)
			out << hex << ",";
		else
			out << hex;
	}
	out << "};\n";
}

void convertStreamToArray(std::string_view name, const char *data, std::size_t length, std::ostream &out) {
	out << "inline const std::array<unsigned char, " << length + 1 << "> " << name << " {";
	for(std::size_t i = 0; i < length; ++i) {
		const std::string hex{std::format("0x{:X}", static_cast<uint8_t>(data[i]))};
		out << hex << ",";
	}
	const std::string hex{std::format("0x{:X}", 0x0)};
	out << hex << "};\n";
}

void convertStreamToString(bool sorted, std::string_view name, std::istream &in, std::ostream &out) {
	out << "inline const std::string " << name << "[] = {";
	std::size_t index{};
	std::size_t length{};
	std::vector<std::string> v;
	while(!in.eof()) {
		std::string line;
		std::getline(in, line);
		if(in) {
			v.push_back(line);
			length += line.length() + 1;
		}
	}
	if(sorted) {
		std::sort(v.begin(), v.end());
	}

	for(auto &line : v) {
		index += line.length() + 1;
		if(line.length() > 0) {
			out << "\"" << convertString(line) << "\"";
			if(index < length)
				out << ",";
		}
	}
	out << "};\n";
}

void stringOutputVector(const std::vector<unsigned char> &v) {
	for(const auto &e : std::views::all(v)) {
		std::cout << e;
	}
}

template <std::size_t N>
void stringOutputArray(std::array<unsigned char, N> &a) {
	for(const auto &i : std::views::all(a)) {
		std::cout << i;
	}
}

bool is_Valid(const std::string &name) {
	std::regex pattern("^[a-zA-Z_][a-zA-Z0-9_]*");
	return std::regex_match(name, pattern);
}

std::string convertString(const std::string &s) {
	std::string temp;
	for(std::size_t i = 0; i < s.length(); ++i) {
		if(s[i] == '\"' || s[i] == '\\') {
			temp += "\\";
			temp += s[i];
		} else {
			temp += s[i];
		}
	}
	return temp;
}
