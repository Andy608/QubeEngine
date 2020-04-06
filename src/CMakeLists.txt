set(QUBEENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/qubeengine)

if (QUBEENGINE_BUILD_RUNNABLE)
    add_executable(QubeEngine
        ${QUBEENGINE_SRC}/main/CommonMain.cpp
        ${QUBEENGINE_SRC}/main/QubeEngineMain.cpp
        ${QUBEENGINE_SRC}/main/Win32Main.cpp
        
        ${QUBEENGINE_SRC}/vulkan_tutorial/VulkanTutorial.cpp
     )
else ()
    add_library(QubeEngine STATIC
        ${QUBEENGINE_SRC}/core/QubeApplication.cpp
        ${QUBEENGINE_SRC}/core/QubeEngine.cpp
        ${QUBEENGINE_SRC}/core/QubeObject.cpp
        
        ${QUBEENGINE_SRC}/main/CommonMain.cpp
        ${QUBEENGINE_SRC}/main/QubeEngineMain.cpp
        ${QUBEENGINE_SRC}/main/Win32Main.cpp
        
        ${QUBEENGINE_SRC}/memory/ITrackable.cpp
        ${QUBEENGINE_SRC}/memory/MemoryTracker.cpp)
endif ()