#pragma once
#include <string>
#include <vector>
void getFiles(std::string path, std::vector<std::string>& files);

bool SnapScreen();
bool SnapScreen(int width, int height, std::string fileName);