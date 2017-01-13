##################################################################
# Settings for the GCC Compiler

##################################################################
# language, set explicit to avoid using some gnu extension
add_compile_options(-std=c++14)

##################################################################
# compiler warnings, should be enabled on every project
# unline on msvc -Wall does not enable all warnings
add_compile_options(-Wall -Wextra -pedantic -Wmain -Wunreachable-code -Wunused -Wunknown-pragmas)
# multiple declaration, shadowing, undef
add_compile_options(-Wshadow)
add_compile_options(-Wundef -Wredundant-decls)
# class/structs and init
add_compile_options(-Wnon-virtual-dtor -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Winit-self -Wuninitialized)
# switch
add_compile_options(-Wswitch-default -Wswitch-enum)
# nullptr and casts warnings (may generate a lot of warnings when interacting with old code or C code)
add_compile_options(-Wzero-as-null-pointer-constant -Wold-style-cast -Wuseless-cast)
add_compile_options(-Wnonnull -Wcast-qual -Wcast-align -Wformat=2)
# arithmetic/numeric warnings
add_compile_options(-Wfloat-equal -Wsign-compare -Wconversion -Wsign-promo)
# logical operations
add_compile_options(-Wlogical-op)
# possible code structure problem
add_compile_options(-Wdisabled-optimization)



option(EFFCXX "Enable Effective C++ Warnings (very noisy, even of correct code)"  OFF)
if(EFFCXX)
	message(" ===== Adding Effective C++ Warnings")
	add_compile_options(-Weffc++)
endif()


##################################################################
# project structure
add_compile_options(-Wmissing-include-dirs)

##################################################################
# errors
add_compile_options(-Werror=format)
add_compile_options(-Werror=format-security)
add_compile_options(-Werror=return-type)
add_compile_options(-Wno-missing-field-initializers)

##################################################################
# additional debug informations
option(DEBUG_ITERATORS "Additional debug information" OFF)
if(DEBUG_ITERATORS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
endif()



##################################################################
# sanitizers (checks made at runtime)
# You can choose only one
set(SanitizerValues "NONE;SANADD;SANUNDEF;SANTHREAD" CACHE STRING
	"List of possible values for the sanitizers")
set(SanValue "SANUNDEF" CACHE STRING
	"SanValue chosen by the user at CMake configure time")
set_property(CACHE SanValue PROPERTY STRINGS ${SanitizerValues})

# not using add_compile_options since since needs flag also for linking
if("${SanValue}" STREQUAL "SANADD")
	message(" ===== Enable sanitizer for addresses ===== ")
	set(SAN_FLAGS "${SAN_FLAGS} -fsanitize=address")
elseif("${SanValue}" STREQUAL "SANUNDEF")
	message(" ===== Enable sanitizer for UB ===== ")
	set(SAN_FLAGS "${SAN_FLAGS} -fsanitize=undefined")
elseif("${SanValue}" STREQUAL "SANTHREAD")
	message(" ===== Enable sanitizer for threads ===== ")
	set(SAN_FLAGS "${SAN_FLAGS} -fsanitize=thread")
endif()


# Check if incompatible with the others
# trap/sanitizer settings
#set(SAN_FLAGS "${SAN_FLAGS}  -ftrapv -fstack-check -fstack-protector")

option(PROFILE "Enable profiling" OFF)
if(PROFILE)
	message(" ===== Enable profiling ===== ")
	add_compile_options(-pg)
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

# add option for relro, pie,
