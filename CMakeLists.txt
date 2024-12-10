cmake_minimum_required (VERSION 3.12)

project ("ViewTether")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(QT_INSTALLATION_PATH "C:/Qt/6.8.0")
set(DEVICE_INSTALLER_64_PATH "${CMAKE_SOURCE_DIR}/thirdparty/usbmmidd_v2/deviceinstaller64.exe ")
add_definitions(-DDEVICE_INSTALLER_64_PATH=\"${DEVICE_INSTALLER_64_PATH}\")
 
# Include headers so that moc can be generated
# https://stackoverflow.com/questions/52413341/why-is-cmake-not-mocing-my-q-object-headers
file (GLOB_RECURSE SOURCES "./ViewTether/src/*.cpp" "./ViewTether/include/*.h" "./ViewTether/form/*.ui")
include_directories(./ViewTether/include)

set(CMAKE_PREFIX_PATH QT_INSTALLATION_PATH)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
qt_standard_project_setup() 
  
add_executable(ViewTether ${SOURCES} "main.cpp" )

set(FFMPEG_INSTALL_DIR "./thirdparty/ffmpeg")

include_directories(${FFMPEG_INSTALL_DIR}/include)
link_directories(${FFMPEG_INSTALL_DIR}/lib)

target_link_libraries(ViewTether PRIVATE avcodec avdevice avfilter avformat avutil postproc swscale swresample libmfx)
 
#add_custom_command(TARGET ViewTether POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${FFMPEG_INSTALL_DIR}/bin $<TARGET_FILE_DIR:ViewTether>
#)
 
#set(CMAKE_MODULE_PATH "C:/vcpkg/installed/x64-windows/share/ffmpeg")
#find_package(FFMPEG REQUIRED)
#target_include_directories(ViewTether PRIVATE ${FFMPEG_INCLUDE_DIRS})
#target_link_directories(ViewTether PRIVATE ${FFMPEG_LIBRARY_DIRS})
#target_link_libraries(ViewTether PRIVATE ${FFMPEG_LIBRARIES})
#message(${FFMPEG_LIBRARIES})

target_link_libraries(ViewTether PRIVATE Qt6::Core Qt6::Widgets Qt6::Network bcrypt mfplat mfuuid secur32)

