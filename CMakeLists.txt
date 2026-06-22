cmake_minimum_required(VERSION 3.5)

project(desktop_todo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        ui/main_window.cpp
        ui/main_window.h
        ui/main_window.ui
        ui/task_list_dialog.h
        ui/task_list_dialog.cpp
        ui/task_list_dialog.ui
        ui/add_dialog.h
        ui/add_dialog.cpp
        ui/add_dialog.ui
        core/event.h
        core/settings_interface.h
        core/qsettings_adapter.h
        core/qsettings_adapter.cpp
        core/config_store.h
        core/config_store.cpp
        core/event_repository.h
        core/event_repository.cpp
        services/update_service.h
        services/update_service.cpp
        )

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(desktop_todo ioc.o
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET desktop_todo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
    if (ANDROID)
        add_library(desktop_todo SHARED
                ${PROJECT_SOURCES}
                )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else ()
        add_executable(desktop_todo ioc.o
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

target_link_libraries(desktop_todo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
find_package(Qt6 COMPONENTS Network REQUIRED)
target_link_libraries(desktop_todo PRIVATE Qt6::Network)

target_include_directories(desktop_todo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/services
        )

set_target_properties(desktop_todo PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
#        WIN32_EXECUTABLE TRUE  #解决qDebug打印不了的问题
        )

install(TARGETS desktop_todo
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(desktop_todo)
endif ()

# -D CMAKE_PREFIX_PATH="D:\Qt\6.2.4\msvc2019_64\lib\cmake" 用msvc的时候，加上cmake那里