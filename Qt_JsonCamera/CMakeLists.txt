cmake_minimum_required(VERSION 3.5)

project(Qt_JsonCamera LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

if(WINDOWS)
        set(BUILD_FOR_WIN TRUE)
endif()

if(LINUX)
    set(BUILD_FOR_LINUX TRUE)
    set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)
endif()



SET("OpenCV_DIR" "E:/coding/opencv/build/x64/vc16/lib")
#SET("OpenCV_DIR" "E:/coding/opencv/build/include")
#SET("OpenCV_LIBS" "E:/coding/opencv/build/x64/vc16/lib")
SET(QT_USE_QTMULTIMEDIA TRUE)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


  add_executable(Qt_JsonCamera
      jsonCamerasettings.cpp
      jsonfile.cpp
      keyboardfilter.cpp
      main.cpp
      drawfacelandmarks.h
      renderface.h
      showvideocapture.cpp
      videoprocessorthread.cpp
      jsonCamerasettings.h
      jsonserializable.h
      jsonsfile.h
      keyboardfilter.h
      showvideocapture.h
      videoprocessorthread.h)



MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")

target_link_libraries(Qt_JsonCamera PRIVATE ${OpenCV_LIBS})
target_link_libraries(Qt_JsonCamera PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Qt_JsonCamera PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(Qt_JsonCamera PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
