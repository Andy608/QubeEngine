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

set(QUBEENGINE_BUILD_RUNNABLE ON)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC ON)

if (APPLE)
elseif()
endif()

set(QUBEENGINE_DEFS)
set(QUBEENGINE_LIBS     
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/vulkan-1.lib)

set(QUBEENGINE_INCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/Vulkan-Headers/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm 
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/spdlog/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_definitions(${QUBEENGINE_DEFS})
include_directories(${QUBEENGINE_INCS})

add_subdirectory(ext/Vulkan-Headers)
add_subdirectory(ext/glm)
add_subdirectory(ext/spdlog)
add_subdirectory(ext/glfw)
add_subdirectory(src)

# against QubeEngine, we need to link against those as well.
target_link_libraries(QubeEngine PRIVATE glfw)
target_link_libraries(QubeEngine PRIVATE ${QUBEENGINE_LIBS})

if (NOT QUBEENGINE_MASTER_PROJECT)
    # This project is included from somewhere else. 
    # Export QUBEENGINE_EXTRA_LIBS variable
    set(QUBEENGINE_EXTRA_INCS ${QUBEENGINE_INCS} PARENT_SCOPE)
    set(QUBEENGINE_EXTRA_DEFS ${QUBEENGINE_DEFS} PARENT_SCOPE)
    set(QUBEENGINE_EXTRA_LIBS ${QUBEENGINE_LIBS} PARENT_SCOPE)
endif ()