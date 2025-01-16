
cmake_minimum_required(VERSION 3.10)
project(ClampEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

set(CMAKE_GENERATOR "MinGW Makefiles")

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

file(GLOB FONT_FILES "${EXTERNAL_DIR}/data/*.ttf")
file(COPY ${FONT_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

configure_file(
    "${EXTERNAL_DIR}/data/Cuprum-Bold.ttf"
    "${CMAKE_CURRENT_BINARY_DIR}/Cuprum-Bold.ttf"
    COPYONLY
)
configure_file(
    "${EXTERNAL_DIR}/data/Oswald-Regular.ttf"
    "${CMAKE_CURRENT_BINARY_DIR}/Oswald-Regular.ttf"
    COPYONLY
)

set(DXSDK_DIR "${EXTERNAL_DIR}/DXSDK")
include_directories(${DXSDK_DIR}/include)
link_directories(${DXSDK_DIR}/lib/x64)

set(GL3W_DIR "${EXTERNAL_DIR}/gl3w")
include_directories(${GL3W_DIR}/Include)

set(GLFW_DIR "${EXTERNAL_DIR}/GLFW")
include_directories(${GLFW_DIR})

set(IMGUI_DIR "${EXTERNAL_DIR}/imgui")
include_directories(${IMGUI_DIR})

set(STB_IMAGE_DIR "${EXTERNAL_DIR}/stb_image")
include_directories(${STB_IMAGE_DIR})


set(IMGUI_NODE_EDITOR_DIR "${EXTERNAL_DIR}/imgui-node-editor")
include_directories(${IMGUI_NODE_EDITOR_DIR})

set(APPLICATION_DIR "${EXTERNAL_DIR}/application")
include_directories(${APPLICATION_DIR}/include)
include_directories(${APPLICATION_DIR}/source)

configure_file(
    "${EXTERNAL_DIR}/application/source/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/external/application/source/config.h"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/external/application/source)

add_library(gl3w STATIC 
    ${GL3W_DIR}/Source/gl3w.c
)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
)

add_library(imgui_node_editor STATIC
    ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor.cpp
    ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor_api.cpp
    ${IMGUI_NODE_EDITOR_DIR}/crude_json.cpp
    ${IMGUI_NODE_EDITOR_DIR}/imgui_canvas.cpp
)

add_library(application STATIC
    ${APPLICATION_DIR}/source/application.cpp
    ${APPLICATION_DIR}/source/entry_point.cpp
    ${APPLICATION_DIR}/source/imgui_impl_dx11.cpp
    ${APPLICATION_DIR}/source/imgui_impl_glfw.cpp
    ${APPLICATION_DIR}/source/imgui_impl_opengl3.cpp
    ${APPLICATION_DIR}/source/imgui_impl_win32.cpp
    ${APPLICATION_DIR}/source/platform_glfw.cpp
    ${APPLICATION_DIR}/source/platform_win32.cpp
    ${APPLICATION_DIR}/source/renderer_dx11.cpp
    ${APPLICATION_DIR}/source/renderer_ogl3.cpp
)

add_executable(ClampEngine 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

target_compile_definitions(imgui_node_editor PUBLIC 
    -DIMGUI_NODE_EDITOR_SHARED
    -DIMGUI_NODE_EDITOR_API_EXPORT
)

target_link_libraries(ClampEngine 
    application
    imgui
    imgui_node_editor
    gl3w
    

    d3d11
    d3dx11
    dxgi
    d3dcompiler
    
    opengl32
    
    kernel32
    user32
    gdi32
    winmm
    imm32
    ole32
    oleaut32
    shell32
    version
    xinput
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")