##################################################################
# Settings for the GCC Compiler

##################################################################
# language, set explicit to avoid using some gnu extension
add_compile_options(-std=c++14)

##################################################################
# compiler warnings, should be enabled on every project
# unlike msvc, -Wall does not enable all warnings

# generic warnings
add_compile_options(-Wall -Wextra -pedantic -Wmain -Wunreachable-code -Wunused -Wunknown-pragmas -Werror=return-type)

# multiple declaration, shadowing, eval undefined identifier
add_compile_options(-Wshadow -Wundef -Wredundant-decls)

# class/structs and init
add_compile_options(-Wnon-virtual-dtor -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Wuninitialized)
if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 6)
	add_compile_options(-Werror=init-self -Werror=memset-transposed-args)
elseif(CMAKE_CXX_COMPILER_VERSION LESSER_EQUAL 4)
	add_compile_options(-Wno-missing-field-initializers) # otherwise S s = {}; issues warning
endif()


# switch/branches
add_compile_options(-Wswitch-default -Wswitch-enum)
if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 6)
	add_compile_options(-Wduplicated-cond)
	if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7)
		add_compile_options(-Wduplicated-branches)
	endif()
endif()

# nullptr and casts warnings (may generate a lot of warnings when interacting with old code or C code)
add_compile_options(-Wzero-as-null-pointer-constant -Wold-style-cast -Wuseless-cast)
add_compile_options(-Wnonnull -Wcast-qual -Wcast-align -Wformat=2)

# arithmetic/numeric warnings
add_compile_options(-Wfloat-equal -Wsign-compare -Wconversion -Wsign-promo)

# logical operations
add_compile_options(-Wlogical-op)

# possible code structure problem
add_compile_options(-Wdisabled-optimization)

# printf
add_compile_options(-Werror=format)
add_compile_options(-Werror=format-security)

# operations on booleans
if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 6)
	add_compile_options(-Werror=bool-compare)
	if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7)
		add_compile_options(-Werror=bool-operation -Wint-in-bool-context)
	endif()
endif()

# inheritance
add_compile_options(-Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override)

# memory
add_compile_options(-Werror=vla -Werror=array-bounds)
if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7)
	add_compile_options(-Walloc-zero -Walloca)
endif()

option(EFFCXX "Enable Effective C++ Warnings (very noisy on correct c++11 code)"  OFF)
if(EFFCXX)
	message(" ===== Adding Effective C++ Warnings")
	add_compile_options(-Weffc++)
endif()


##################################################################
# project structure
add_compile_options(-Wmissing-include-dirs)


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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif("${SanValue}" STREQUAL "SANUNDEF")
	message(" ===== Enable sanitizer for UB ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
elseif("${SanValue}" STREQUAL "SANTHREAD")
	message(" ===== Enable sanitizer for threads ===== ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
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
# mitigations
# abort and provide some useful message instead of ignoring errors, crash at another random place or leave some vulnerability open
option(FORTIFY "Enable fortify sources (already enabled in release mode)" OFF)
if(FORTIFY)
	message(" ===== Enable fortify sources (always enabled in release mode) ===== ")
	add_definitions(-D_FORTIFY_SOURCE=2 -O2)
endif()
# add check if compiling with -O1, in that case we should use -D_FORTIFY_SOURCE=1...
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2") # =1 when using -O1..

# add option for relro, pie,

