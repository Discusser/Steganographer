#pragma once

#include <string>

#include "./image.hpp"

Image generate_random_image(std::string path, int width, int height);
int parse_command_line_arguments(int *argc, char **argv[]);
