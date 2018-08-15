#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>

void list_files_recursive(std::string init_path, HDC &hdc, int* x, int* y);