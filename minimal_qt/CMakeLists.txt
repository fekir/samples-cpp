cmake_minimum_required(VERSION 3.3)

project(MinimalQt CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(SOURCE_FILES
	mainwindow.ui
	mainwindow.hpp
	mainwindow.cpp
)

add_definitions(${Qt5Widgets_DEFINITIONS})

if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 main.cpp ${SOURCE_FILES})
else()
	add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})
endif()

set(QT_LIBRARIES_TO_LINK
	Qt5::Core Qt5::Gui Qt5::Widgets
)

target_link_libraries(${PROJECT_NAME} 
	${QT_LIBRARIES_TO_LINK}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# Instruct CMake to run moc automatically
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOUIC TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTORCC TRUE)

