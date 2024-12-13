cmake_minimum_required (VERSION 3.12)

project ("ViewTether")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(DEVICE_INSTALLER_64_PATH "${CMAKE_SOURCE_DIR}/thirdparty/usbmmidd_v2/deviceinstaller64.exe")
add_definitions(-DDEVICE_INSTALLER_64_PATH=\"${DEVICE_INSTALLER_64_PATH}\")

set(FFMPEG_INSTALLATION_PATH "${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg")

#set(QT_INSTALLATION_PATH "C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6")

#set(Qt6_DIR QT_INSTALLATION_PATH)
#find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
#qt_standard_project_setup() 

# Include headers so that moc can be generated
# https://stackoverflow.com/questions/52413341/why-is-cmake-not-mocing-my-q-object-headers
#file (GLOB_RECURSE SOURCES "./ViewTether/src/*.cpp" "./ViewTether/include/*.h" "./ViewTether/form/*.ui")
#include_directories(./ViewTether/include)
#qt_add_executable(ViewTether ${SOURCES} "main.cpp")

add_executable(ViewTether "main.cpp")

include_directories(${FFMPEG_INSTALLATION_PATH}/include)
link_directories(${FFMPEG_INSTALLATION_PATH}/lib)
target_link_libraries(ViewTether PRIVATE avcodec avdevice avfilter avformat avutil libmfx pkgconf swresample swscale)
 
#target_link_libraries(ViewTether PRIVATE Qt6::Core Qt6::Widgets Qt6::Network bcrypt mfplat mfuuid secur32)
