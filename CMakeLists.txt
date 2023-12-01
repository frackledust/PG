cmake_minimum_required(VERSION 3.26)
project(pg1)

set(CMAKE_CXX_STANDARD 14)

# header files
include_directories(src/libs/gl3w/include)
include_directories(src/libs/embree/include)
include_directories(src/libs/freeimage/include)
include_directories(src/libs/imgui/include)

include_directories(src/pg/pg1_embree)

# static libraries
link_directories(src/libs/embree/lib)
link_directories(src/libs/freeimage/lib)
link_directories(src/libs/glfw)


set(IMGUI
        src/libs/imgui/imgui.cpp
        src/libs/imgui/imgui_demo.cpp
        src/libs/imgui/imgui_draw.cpp
        src/libs/imgui/imgui_impl_dx11.cpp
        src/libs/imgui/imgui_impl_dx12.cpp
        src/libs/imgui/imgui_impl_glfw.cpp
        src/libs/imgui/imgui_impl_opengl3.cpp
        src/libs/imgui/imgui_impl_win32.cpp
)

set(GL13W src/libs/gl3w/gl3w.c)
file(GLOB SOURCES "src/pg/pg1_embree/*.cpp")

add_executable(pg1
        ${IMGUI}
        ${GL13W}
        ${OTHER}
        ${SOURCES})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(pg1 OpenMP::OpenMP_CXX)
    endif()
endif()

target_link_libraries(pg1 d3d11 glfw3 embree3 FreeImaged d3d11 d3d12 d3dcompiler)