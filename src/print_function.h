#ifndef PRINT_FUNCTION_H
#define PRINT_FUNCTION_H

// std
#include <string>
#include <iostream>

void main_info(const std::string& section, const std::string& msg);
void main_error(const std::string& section, const std::string& msg);

#define MAIN_INFO_1(msg) main_info(__FILE_NAME__, msg)
#define MAIN_ERROR_1(msg) main_error(__FILE_NAME__, msg)

#endif