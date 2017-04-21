cmake_minimum_required(VERSION 3.2)
project(SpudEngine2)

# Include all of the .hpp, .cpp and .h files in the SRC folder
FILE(GLOB_RECURSE HPPS ${CMAKE_SOURCE_DIR}/src/*.hpp)
FILE(GLOB_RECURSE HS ${CMAKE_SOURCE_DIR}/src/*.h)
FILE(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Get the paths for the libraries
include_directories( BEFORE SYSTEM
    ${CMAKE_SOURCE_DIR}/GLFW/include/
    ${CMAKE_SOURCE_DIR}/FreeImage/include/
    ${CMAKE_SOURCE_DIR}/PhysX/include/
    ${CMAKE_SOURCE_DIR}/OpenAL/include
    ${CMAKE_SOURCE_DIR}/glm/
    ${CMAKE_SOURCE_DIR}/src/)

set( CMAKE_FRAMEWORK_PATH
    ${CMAKE_FRAMEWORK_PATH})

set( CMAKE_LIBRARY_PATH
    ${CMAKE_SOURCE_DIR}/GLFW/lib/
    ${CMAKE_SOURCE_DIR}/FreeImage/lib/
    ${CMAKE_SOURCE_DIR}/PhysX/lib/
    ${CMAKE_SOURCE_DIR}/OpenAL/lib/
    ${CMAKE_LIBRARY_PATH})

# OpenAL
find_library( OPENAL_1_LIBRARY openal.1)
message( STATUS "OPENAL_1_LIBRARY is ${OPENAL_1_LIBRARY}")

# macOS specific frameworks
if (APPLE)

    find_library( COCOA_LIBRARY Cocoa)
    message( STATUS "COCOA_LIBRARY is ${COCOA_LIBRARY}")
    find_library( CORE_VIDEO_LIBRARY CoreVideo)
    message( STATUS "CORE_VIDEO_LIBRARY is ${CORE_VIDEO_LIBRARY}")
    find_library( FOUNDATION_LIBRARY Foundation)
    message( STATUS "FOUNDATION_LIBRARY is ${FOUNDATION_LIBRARY}")
    find_library( IOKIT_LIBRARY IOKit)
    message( STATUS "IOKIT_LIBRARY is ${IOKIT_LIBRARY}")
    find_library( OPEN_GL_LIBRARY OpenGL)
    message( STATUS "OPEN_GL_LIBRARY is ${OPEN_GL_LIBRARY}")

    set( DEPENDENCIES
        ${COCOA_LIBRARY}
        ${CORE_VIDEO_LIBRARY}
        ${FOUNDATION_LIBRARY}
        ${IOKIT_LIBRARY}
        ${OPENAL_1_LIBRARY}
        ${OPEN_GL_LIBRARY}
        )

endif()


# PhysX
find_library( LOW_LEVEL_LIBRARY LowLevel)
message( STATUS "LOW_LEVEL_LIBRARY is ${LOW_LEVEL_LIBRARY}")
find_library( LOW_LEVEL_CLOTH_LIBRARY LowLevelCloth)
message( STATUS "LOW_LEVEL_CLOTH_LIBRARY is ${LOW_LEVEL_CLOTH_LIBRARY}")
find_library( PHYS_X3_LIBRARY PhysX3)
message( STATUS "PHYS_X3_LIBRARY is ${PHYS_X3_LIBRARY}")
find_library( PHYS_X3_CHARACTER_KINEMATIC_LIBRARY PhysX3CharacterKinematic)
message( STATUS "PHYS_X3_CHARACTER_KINEMATIC_LIBRARY is ${PHYS_X3_CHARACTER_KINEMATIC_LIBRARY}")
find_library( PHYS_X3_COMMON_LIBRARY PhysX3Common)
message( STATUS "PHYS_X3_COMMON_LIBRARY is ${PHYS_X3_COMMON_LIBRARY}")
find_library( PHYS_X3_COOKING_LIBRARY PhysX3Cooking)
message( STATUS "PHYS_X3_COOKING_LIBRARY is ${PHYS_X3_COOKING_LIBRARY}")
find_library( PHYS_X3_EXTENSIONS_LIBRARY PhysX3Extensions)
message( STATUS "PHYS_X3_EXTENSIONS_LIBRARY is ${PHYS_X3_EXTENSIONS_LIBRARY}")
find_library( PHYS_X3_VEHICLE_LIBRARY PhysX3Vehicle)
message( STATUS "PHYS_X3_VEHICLE_LIBRARY is ${PHYS_X3_VEHICLE_LIBRARY}")
find_library( PHYS_XPROFILE_SDK_LIBRARY PhysXProfileSDK)
message( STATUS "PHYS_XPROFILE_SDK_LIBRARY is ${PHYS_XPROFILE_SDK_LIBRARY}")
find_library( PHYS_XVISUAL_DEBUGGER_SDK_LIBRARY PhysXVisualDebuggerSDK)
message( STATUS "PHYS_XVISUAL_DEBUGGER_SDK_LIBRARY is ${PHYS_XVISUAL_DEBUGGER_SDK_LIBRARY}")
find_library( PVD_RUNTIME_LIBRARY PvdRuntime)
message( STATUS "PVD_RUNTIME_LIBRARY is ${PVD_RUNTIME_LIBRARY}")
find_library( PX_TASK_LIBRARY PxTask)
message( STATUS "PX_TASK_LIBRARY is ${PX_TASK_LIBRARY}")
find_library( SCENE_QUERY_LIBRARY SceneQuery)
message( STATUS "SCENE_QUERY_LIBRARY is ${SCENE_QUERY_LIBRARY}")
find_library( SIMULATION_CONTROLLER_LIBRARY SimulationController)
message( STATUS "SIMULATION_CONTROLLER_LIBRARY is ${SIMULATION_CONTROLLER_LIBRARY}")

# Boost
find_library( BOOST_SIGNALS_LIBRARY boost_signals)
message( STATUS "BOOST_SIGNALS_LIBRARY is ${BOOST_SIGNALS_LIBRARY}")
find_library( BOOST_SYSTEM_LIBRARY boost_system)
message( STATUS "BOOST_SYSTEM_LIBRARY is ${BOOST_SYSTEM_LIBRARY}")
find_library( BOOST_THREAD_LIBRARY boost_thread)
message( STATUS "BOOST_THREAD_LIBRARY is ${BOOST_THREAD_LIBRARY}")

# FreeImage
find_library( FREEIMAGE_LIBRARY freeimage)
message( STATUS "FREEIMAGE_LIBRARY is ${FREEIMAGE_LIBRARY}")

# GLFW
find_library( GLFW3_LIBRARY glfw3)
message( STATUS "GLFW3_LIBRARY is ${GLFW3_LIBRARY}")

set( STATIC_DEPENDENCIES
    ${BOOST_SIGNALS_LIBRARY}
    ${BOOST_SYSTEM_LIBRARY}
    ${BOOST_THREAD_LIBRARY}
    ${FREEIMAGE_LIBRARY}
    ${GLFW3_LIBRARY}
    ${LOW_LEVEL_LIBRARY}
    ${LOW_LEVEL_CLOTH_LIBRARY}
    ${PHYS_X3_LIBRARY}
    ${PHYS_X3_CHARACTER_KINEMATIC_LIBRARY}
    ${PHYS_X3_COMMON_LIBRARY}
    ${PHYS_X3_COOKING_LIBRARY}
    ${PHYS_X3_EXTENSIONS_LIBRARY}
    ${PHYS_X3_VEHICLE_LIBRARY}
    ${PHYS_XPROFILE_SDK_LIBRARY}
    ${PHYS_XVISUAL_DEBUGGER_SDK_LIBRARY}
    ${PVD_RUNTIME_LIBRARY}
    ${PX_TASK_LIBRARY}
    ${SCENE_QUERY_LIBRARY}
    ${SIMULATION_CONTROLLER_LIBRARY}
    )

if (APPLE)

    # macOS specific final steps
    # Icon
    set( ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns" )

    set_source_files_properties(
        ${ICON_PATH}
        PROPERTIES
            MACOSX_PACKAGE_LOCATION
            Resources
    )

    # Bundle
    add_executable( Spud-Engine-2 MACOSX_BUNDLE
        ${HPPS}
        ${HS}
        ${SOURCES}
        ${ICON_PATH}
        ${Resources}
    )

    # Resources
    add_custom_command(TARGET Spud-Engine-2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Resources "${CMAKE_CURRENT_BINARY_DIR}/Spud-Engine-2.app/Contents/Resources/")

    # Info.plist
    set_target_properties( Spud-Engine-2 PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Spud-Engine-2-Info.plist.in" )

elseif(WIN32)

    add_executable( Spud-Engine-2
        ${HPPS}
        ${HS}
        ${SOURCES}
        ${RESOURCES}
        ${ICON_PATH}
    )

    add_custom_command(TARGET Spud-Engine-2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Resources "${CMAKE_CURRENT_BINARY_DIR}/Resources/")

endif()

target_link_libraries( Spud-Engine-2
    ${BEGIN_ALL_LOAD}
    ${STATIC_DEPENDENCIES}
    ${END_ALL_LOAD}
    ${DEPENDENCIES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0 -std=gnu++11 -stdlib=libc++ -fmodules -gmodules -fmodules-prune-interval=86400 -fmodules-prune-after=345600 -Wnon-modular-include-in-framework-module -Werror=non-modular-include-in-framework-module -Wno-trigraphs -fpascal-strings -O0 -fno-common -Wno-missing-field-initializers -Wno-missing-prototypes -Werror=return-type -Wunreachable-code -Werror=deprecated-objc-isa-usage -Werror=objc-root-class -Wno-non-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wconditional-uninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wshorten-64-to-32 -Wno-newline-eof -Wno-c++11-extensions -DDEBUG=1")
