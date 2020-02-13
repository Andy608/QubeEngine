cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(QubeEngine)

if (NOT DEFINED QUBEENGINE_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(QUBEENGINE_MASTER_PROJECT ON)
    else()
        set(QUBEENGINE_MASTER_PROJECT OFF)
    endif()
endif ()

set(Vulkan_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/Vulkan-Headers/include)
set(Vulkan_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/vulkan-1.lib)

# Disable building extras we won't need (pure C++ project)
set(NANOGUI_BUILD_PYTHON    OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL         OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED     ON CACHE BOOL " " FORCE)

# Add the configurations from nanogui
#if (APPLE)
#    set(NANOGUI_ROOT ext/osx/nanogui)
#    set(NANOGUI_BUILD_EXAMPLE  OFF CACHE BOOL " " FORCE)
#else ()
    set(NANOGUI_ROOT ext/nanogui)
    set(NANOGUI_BUILD_EXAMPLE ON)
    set(NANOGUI_VULKAN_BACKEND ON)
#endif ()

if (APPLE)
elseif()
endif()
add_subdirectory(ext/Vulkan-Headers)
add_subdirectory(${NANOGUI_ROOT})
add_subdirectory(ext/glm)
add_subdirectory(ext/spdlog)

# For reliability of parallel build, make the NanoGUI targets dependencies
set_property(TARGET glfw glfw_objects nanogui PROPERTY FOLDER "dependencies")

set(QUBEENGINE_DEFS
        ${NANOGUI_EXTRA_DEFS})

set(QUBEENGINE_LIBS 
    nanogui
    ${Vulkan_LIBRARY}
    ${NANOGUI_EXTRA_LIBS})

message("VULKAN ${Vulkan_INCLUDE_DIR}")

set(QUBEENGINE_INCS 
    ${Vulkan_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${NANOGUI_ROOT}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm/glm 
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/spdlog/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${NANOGUI_EXTRA_INCS})

add_definitions(${QUBEENGINE_DEFS})
include_directories(${QUBEENGINE_INCS})

set(QUBEENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/qubeengine)

add_library(QubeEngine STATIC
    ${QUBEENGINE_SRC}/core/QubeApplication.cpp
    ${QUBEENGINE_SRC}/core/QubeEngine.cpp
    ${QUBEENGINE_SRC}/core/QubeObject.cpp
    
    ${QUBEENGINE_SRC}/main/CommonMain.cpp
    ${QUBEENGINE_SRC}/main/QubeEngineMain.cpp
    ${QUBEENGINE_SRC}/main/Win32Main.cpp
    
    ${QUBEENGINE_SRC}/memory/ITrackable.cpp
    ${QUBEENGINE_SRC}/memory/MemoryTracker.cpp)

# Lastly, additional libraries may have been built for you.  In addition to linking
# against QubeEngine, we need to link against those as well.
target_link_libraries(QubeEngine PRIVATE ${QUBEENGINE_LIBS})

if (NOT QUBEENGINE_MASTER_PROJECT)
    # This project is included from somewhere else. Export QUBEENGINE_EXTRA_LIBS variable
    set(QUBEENGINE_EXTRA_INCS ${QUBEENGINE_INCS} PARENT_SCOPE)
    set(QUBEENGINE_EXTRA_DEFS ${QUBEENGINE_DEFS} PARENT_SCOPE)
    set(QUBEENGINE_EXTRA_LIBS ${QUBEENGINE_LIBS} PARENT_SCOPE)
endif ()