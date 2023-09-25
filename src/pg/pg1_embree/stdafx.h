#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS

// std libs
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <tchar.h>
#include <vector>
#include <map>
#include <random>
#include <functional>

#include <xmmintrin.h>
#include <pmmintrin.h>

// Intel Embree 3.6.1 (high performance ray tracing kernels)
#include <embree3/rtcore.h>

// Dear ImGui (bloat-free graphical user interface library for C++)
#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
