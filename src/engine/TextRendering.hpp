#pragma once
#include <GLFW/glfw3.h>
#include <string>

void TextRendering_Init();
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
