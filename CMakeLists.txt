cmake_minimum_required(VERSION 3.16)

project(desktop_todo VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml Quick QuickControls2 Network)

set(PROJECT_SOURCES
    main.cpp
    ui/application_controller.h
    ui/application_controller.cpp
    ui/task_list_model.h
    ui/task_list_model.cpp
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

qt_add_executable(desktop_todo
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    Desktop.rc
)

target_link_libraries(desktop_todo PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
)

qt_add_qml_module(desktop_todo
    URI DesktopPet
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/PetWindow.qml
        qml/TaskListDialog.qml
        qml/AddDialog.qml
        qml/TaskListItem.qml
        qml/Theme.qml
    RESOURCE_PREFIX "/"
)

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
#    WIN32_EXECUTABLE TRUE  #解决qDebug打印不了的问题
)

install(TARGETS desktop_todo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
