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

std::tuple<int, int, int> get_vertex_elements(std::string s);
std::tuple<int, int, int> to_index(const std::tuple<int, int, int>& vertex_elements,
                                   const int v_offset_idx, const int vt_offset_idx,
                                   const int vn_offset_idx);
bool load_objmtl_file(const std::string file_path, std::map<std::string, std::string>& materials);
bool load_obj_file(const std::string file_path, std::vector<Object*>& objects);

#endif
