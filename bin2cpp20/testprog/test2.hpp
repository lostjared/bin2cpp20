#ifndef __ARR_H_HPP_var2
#define __ARR_H_HPP_var2
#include<string>

inline const std::wstring var2[] = {L"CXXFLAGS = -std=c++20 ",L"LDFLAGS = ",L"CPP_FILES := $(wildcard *.cpp)",L"OBJ_FILES := $(CPP_FILES:.cpp=.o)",L"OUTPUT_NAME =  bin2cpp20",L"DEPS := $(wildcard *.hpp)",L"%.o: %.cpp $(DEPS) ",L"	$(CXX) $(CXXFLAGS) -c $< -o $@",L"$(OUTPUT_NAME): $(OBJ_FILES)",L"	$(CXX)  $^ -o $@ $(LDFLAGS)",L"all: $(OUTPUT_NAME)",L"install: all",L"	cp $(OUTPUT_NAME) /usr/local/bin",L"clean:",L"	rm -f $(OUTPUT_NAME) $(OBJ_FILES)",L".PHONY: all install clean"};
inline unsigned long var2_length {16};


#endif
