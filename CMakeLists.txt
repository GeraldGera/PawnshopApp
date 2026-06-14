cmake_minimum_required(VERSION 3.16)

project(PawnshopApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 QUIET COMPONENTS Widgets Sql)
if(Qt6_FOUND)
    set(QT_PACKAGE Qt6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)
    set(QT_PACKAGE Qt5)
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/Config/AppConfig.cpp
    src/Config/AppConfig.h
    src/Controller/AuthController.cpp
    src/Controller/AuthController.h
    src/Controller/DataController.cpp
    src/Controller/DataController.h
    src/Controller/ProfileController.cpp
    src/Controller/ProfileController.h
    src/Model/DatabaseManager.cpp
    src/Model/DatabaseManager.h
    src/Model/Entities.cpp
    src/Model/Entities.h
    src/Model/PasswordUtils.cpp
    src/Model/PasswordUtils.h
    src/Model/Repositories.cpp
    src/Model/Repositories.h
    src/View/CrudPage.cpp
    src/View/CrudPage.h
    src/View/LoginWindow.cpp
    src/View/LoginWindow.h
    src/View/MainWindow.cpp
    src/View/MainWindow.h
    src/View/RecordDialog.cpp
    src/View/RecordDialog.h
    src/View/RegisterDialog.cpp
    src/View/RegisterDialog.h
)

add_executable(PawnshopApp WIN32 ${PROJECT_SOURCES})
target_include_directories(PawnshopApp PRIVATE src)
target_link_libraries(PawnshopApp PRIVATE ${QT_PACKAGE}::Widgets ${QT_PACKAGE}::Sql)

if(MSVC)
    target_compile_options(PawnshopApp PRIVATE /utf-8)
endif()

if(Qt6_FOUND)
    qt_finalize_executable(PawnshopApp)
endif()
