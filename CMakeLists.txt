cmake_minimum_required(VERSION 3.16)
project(arm-pi-tweaker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

# Find OpenMP for llama.cpp
find_package(OpenMP REQUIRED)

# Enable Qt's automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add llama.cpp include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty/llamacpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty/llamacpp/ggml-include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty/llamacpp/common)

# Find llama.cpp static libraries
set(LLAMACPP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty/lib)
set(LLAMA_LIB ${LLAMACPP_LIB_DIR}/libllama.a)
set(GGML_LIB ${LLAMACPP_LIB_DIR}/libggml.a)
set(GGML_BASE_LIB ${LLAMACPP_LIB_DIR}/libggml-base.a)
set(GGML_CPU_LIB ${LLAMACPP_LIB_DIR}/libggml-cpu.a)

# Source files
set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    widgets/upgradewidget.cpp
    widgets/upgradewidget.h
    systemmanager.cpp
    systemmanager.h
    gpumanager.cpp
    gpumanager.h
    kernelmanager.cpp
    kernelmanager.h
    storagemanager.cpp
    storagemanager.h
    widgets/customimagewizard.cpp
    widgets/customimagewizard.h
    imagebuilder.cpp
    imagebuilder.h
    widgets/uefitab.cpp
    widgets/uefitab.h
    widgets/rockytab.cpp
    widgets/rockytab.h
    widgets/welcometab.cpp
    widgets/welcometab.h
    widgets/backupwizard.cpp
    widgets/backupwizard.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link Qt libraries and llama.cpp static libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core Qt6::Widgets Qt6::Network
    ${LLAMA_LIB}
    ${GGML_LIB}
    ${GGML_BASE_LIB}
    ${GGML_CPU_LIB}
    OpenMP::OpenMP_CXX
    pthread
    m
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)