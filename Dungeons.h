#pragma once


// API стандартной библиотеки
#include <iostream>
#include <thread>
#include <chrono>


// Платформозависимые файлы
#ifdef _WIN32
#include "windows.h"
#endif


// Исходники проекта
#include "./time.h"
#include "./Random.h"



#include "./tui/Terminal.h"
#include "./tui/UnicodeCharMatrix.h"
#include "./tui/StyleMatrix.h"
#include "./tui/frame_building.h"
