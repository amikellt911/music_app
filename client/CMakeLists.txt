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

set(PROJECT_SOURCES
  src/widgets/btform.cpp
  src/widgets/client.cpp
  src/widgets/commonpage.cpp
  src/widgets/listitembox.cpp
  src/widgets/recbox.cpp
  src/widgets/recboxitem.cpp
  src/widgets/volumetool.cpp
  src/model/music.cpp
  src/model/musiclist.cpp

)
set(PROJECT_HEADERS
  src/widgets/btform.h
  src/widgets/client.h
  src/widgets/commonpage.h
  src/widgets/listitembox.h
  src/widgets/recbox.h
  src/widgets/recboxitem.h
  src/widgets/volumetool.h
  src/model/music.h
  src/model/musiclist.h
)


set(PROJECT_FORMS
  src/ui/btform.ui
  src/ui/client.ui
  src/ui/commonpage.ui
  src/ui/listitembox.ui
  src/ui/recbox.ui
  src/ui/recboxitem.ui
  src/ui/volumetool.ui
)
set(PROJECT_RESOURCES
    resources/Resource.qrc
)


set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/src/ui)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

  add_executable(client
  main.cpp
  ${PROJECT_SOURCES}
  ${PROJECT_HEADERS}
  ${PROJECT_FORMS}
  ${PROJECT_RESOURCES}
  )

  target_include_directories(client PRIVATE
  "${PROJECT_SOURCE_DIR}/src/widgets"
  "${PROJECT_SOURCE_DIR}/src/model"
)

target_link_libraries(client PRIVATE Qt5::Widgets)
