# bin2cpp20 - Binary to C++20 Source Conversion Tool

## Overview

`bin2cpp20` is a versatile command-line utility designed to convert binary files/string data into C++20 compliant source code. This enables the embedding of binary/string data directly within C++ applications in a portable and efficient manner. The tool offers flexibility in output formats, including vectors, arrays, and strings, and supports additional options like sorting and Unicode handling.

## Requirements

- Fully C++20 compliant environment

## Compilation

To compile `bin2cpp20`, use the following command:

```sh
make -f Makefile.ux 
```
or
```sh
mkdir build && cd build
cmake ..
make -j4
sudo make install 
```


## Usage

The basic usage of the tool can be invoked as follows:

```sh
bin2cpp20 [options]
```

### Key Features

- **Input from file or standard input**: Specify an input binary file or use standard input.
- **Output to standard output or file**: Direct the converted C++ source to standard output or a specified file.
- **Variable naming**: Define custom variable names for the generated code.
- **Output formats**: Choose between vector, array, or string representations of the binary data.
- **Sorting and Unicode options**: Sort strings in the output or handle Unicode data correctly.

### Options

- `-i, --input <file/stdin>`: Specify the input source. Use "stdin" to read from standard input.
- `-o, --output <file>`: Specify the output file. If not provided, output will be directed to standard output.
- `-v, --variable <name>`: Set the name of the generated variable. This is required.
- `-s, --string output`: Generate output as a string.
- `-z, --sort`: Sort the output string. Applicable only with string output.
- `-u, --unicode`: Treat the input as Unicode. Applicable only with string output.
- `-h, --help`: Display the help message and exit.

### Examples

**Convert a binary file to a arrau:**

```sh
bin2cpp20 -i path/to/input/file -o path/to/output.hpp -v variable_name
```

**Convert standard input to a sorted Unicode string:**

```sh
cat path/to/input | bin2cpp20 -i stdin -s -u -z -v variable_name -o path/to/output.hpp
```

## Additional Notes

- Ensure variable names are valid C++ identifiers.
- Input files are read in binary mode, preserving the integrity of binary data.
- The tool automatically appends `.hpp` to output filenames if not present.

For issues or contributions, please visit the [project repository](#).
