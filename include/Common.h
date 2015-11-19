#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>

using namespace std;

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_ttf.h>
#include <SDL_opengl.h>
#include <fbxsdk.h>

//maths headers
#include <glm/glm.hpp>
using namespace glm;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const string ASSET_PATH = "assets";
const string SHADER_PATH = "/shaders";
const string TEXTURE_PATH = "/textures";
const string FONT_PATH = "/fonts";
const string MODEL_PATH = "/models";

#endif