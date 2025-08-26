cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
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

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
  add_library(client SHARED
    main.cpp
    client.cpp
    client.h
    client.ui
    Resource.qrc
    btform.cpp
    btform.h
    btform.ui
    recbox.cpp
    recbox.h
    recbox.ui
    recboxitem.cpp
    recboxitem.h
    recboxitem.ui
    commonpage.cpp
    commonpage.h
    commonpage.ui
    listitembox.cpp
    listitembox.h
    listitembox.ui
    volumetool.cpp
    volumetool.h
    volumetool.ui
  )
else()
  add_executable(client
    main.cpp
    client.cpp
    client.h
    client.ui
    Resource.qrc
    btform.cpp
    btform.h
    btform.ui
    recbox.cpp
    recbox.h
    recbox.ui
    recboxitem.cpp
    recboxitem.h
    recboxitem.ui
    commonpage.cpp
    commonpage.h
    commonpage.ui
    listitembox.cpp
    listitembox.h
    listitembox.ui
    volumetool.cpp
    volumetool.h
    volumetool.ui
  )
endif()

target_link_libraries(client PRIVATE Qt5::Widgets)
