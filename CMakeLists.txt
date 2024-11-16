cmake_minimum_required (VERSION 3.12)

project ("ViewTether")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

include_directories(./ViewTether/include)

# Include headers so that moc can be generated
# https://stackoverflow.com/questions/52413341/why-is-cmake-not-mocing-my-q-object-headers
file (GLOB_RECURSE SOURCES "./ViewTether/src/*.cpp" "./ViewTether/include/*.h" "./ViewTether/include/*.ui")

# QT Setup
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0") # path to qt installation
find_package(Qt6 COMPONENTS Core Widgets Network)
qt_standard_project_setup() 
 
set(CMAKE_MODULE_PATH "C:/vcpkg/installed/x64-windows/share/ffmpeg" ${CMAKE_MODULE_PATH})
find_package(FFMPEG REQUIRED)
 
qt_add_executable(ViewTether ${SOURCES} "main.cpp")

target_include_directories(ViewTether PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(ViewTether PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(ViewTether PRIVATE ${FFMPEG_LIBRARIES})

target_link_libraries(ViewTether PRIVATE Qt6::Core Qt6::Widgets Qt6::Network bcrypt mfplat mfuuid secur32)
