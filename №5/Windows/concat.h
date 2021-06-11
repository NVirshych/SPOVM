#pragma once

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

#ifdef CONCAT_EXPORTS
#define CONCAT_API __declspec(dllexport)
#else
#define CONCAT_API __declspec(dllimport)
#endif

extern "C" CONCAT_API void concat();
