cmake_minimum_required(VERSION 2.8)

project(minimal_mfc_async CXX)

add_definitions(-DWIN32_LEAN_AND_MEAN)                # removes rarely used header from windows.h (simply include those headers when needed)
add_definitions(-DNOMINMAX)                           # removes min and max macro, use std::min and std::max
add_definitions(-DSTRICT)                             # enable strict mode (adds more typesafety, for example replacing some typedef with structs)

add_definitions(-DNTDDI_VERSION=NTDDI_VISTA)          # set minimal version of windows to windows vista
add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)    # set minimal version of windows to windows vista
add_definitions(-DWINVER=_WIN32_WINNT_VISTA)          # set minimal version of windows to windows vista

add_definitions(-D_UNICODE -DUNICODE)                 # use unicode apis, the ansi api behaves incorrectly when using letters outside from local codepage

add_definitions(-D_ATL_CSTRING_EXPLICIT_CONSTRUCTORS) # make CString constructor explicit
add_definitions(-D_ATL_ALL_WARNINGS)                  # otherwise ATL may globally disable some warning
add_definitions(-D_SECURE_ATL=1)                      # remove deprecated functions

# usage of MFC
set(CMAKE_MFC_FLAG 1) # 1 = static, 2 = shared
add_definitions(-D_AFXDLL)

set(SOURCE_FILES
	mydialog.hpp
	mydialog.cpp
)

set(RES_FILES
	resource.rc
	resource.h
)

add_executable(${PROJECT_NAME} WIN32
	main.hpp main.cpp
	${SOURCE_FILES}
	${RES_FILES}
)

