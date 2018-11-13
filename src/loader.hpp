#ifndef _LOADER_H_
#define _LOADER_H_

#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "surface.hpp"
#include "utility.hpp"

bool load_ply_file(const std::string file_path, Surface* surface);

#endif
