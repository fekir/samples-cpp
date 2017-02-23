##################################################################
# Settings for the Clang Compiler

##################################################################
# language, set explicit to avoid using some extension
add_compile_options(-std=c++14)

##################################################################
# compiler warnings, should be enabled on every project
add_compile_options(-Weverything)
add_compile_options(-Wno-c++98-compat -Wno-c++98-compat-pedantic)  # since writing c++11 or greater
add_compile_options(-Wno-padded)
add_compile_options(-Wno-weak-vtables)

##################################################################
# errors
add_compile_options(-pedantic-errors)                  # treat language extension as errors
add_compile_options(-Wno-system-headers)               # disable warnings from system headers
add_compile_options(-Werror=extra-tokens)              # treat extra tokens as errors

##################################################################
# sanitizers (checks made at runtime)
# You can choose only one
set(SanitizerValues "NONE;SANADD;SANTHREAD;SANMEM;SANUNDEF;SANFLOW;SANCFI;SANSTACK" CACHE STRING
	"List of possible values for the sanitizers")
set(SanValue "SANUNDEF" CACHE STRING
	"SanValue chosen by the user at CMake configure time")
set_property(CACHE SanValue PROPERTY STRINGS ${SanitizerValues})

# not using add_compile_options since since needs flag also for linking
if("${SanValue}" STREQUAL "SANADD")
	message(" ===== Enable sanitizer for addresses ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif("${SanValue}" STREQUAL "SANTHREAD")
	message(" ===== Enable sanitizer for threads ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
elseif("${SanValue}" STREQUAL "SANMEM")
	message(" ===== Enable sanitizer for memory ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
elseif("${SanValue}" STREQUAL "SANUNDEF")
	message(" ===== Enable sanitizer for UB ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
elseif("${SanValue}" STREQUAL "SANFLOW")
	message(" ===== Enable sanitizer for dataflow ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=dataflow")
elseif("${SanValue}" STREQUAL "SANCFI")
	message(" ===== Enable sanitizer for control flow integrity ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=cfi")
	add_compile_options(-flto)
elseif("${SanValue}" STREQUAL "SANSTACK")
	message(" ===== Enable sanitizer for stack protection ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=safe-stack")
endif()

##################################################################
# mitigations (
# abort and provide some useful message instead of ignoring errors, crash at another random place or leave some vulnerability open
option(FORTIFY "Enable fortify sources (already enabled in release mode)" OFF)
if(FORTIFY)
	message(" ===== Enable fortify sources (always enabled in release mode) ===== ")
	add_definitions(-D_FORTIFY_SOURCE=2 -O2)
endif()
# if compiling with -O1 we should use -D_FORTIFY_SOURCE=1...
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2") # =1 when using -O1..

add_compile_options(-fstack-protector-all)

# add option for relro, pie,
