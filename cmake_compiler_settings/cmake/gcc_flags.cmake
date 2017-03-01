##################################################################
# Settings for the GCC Compiler

##################################################################
# language, set explicit to avoid using some gnu extension
add_compile_options(-std=c++14)

##################################################################
# compiler warnings, should be enabled on every project
# unlike msvc, -Wall does not enable all warnings

# generic warnings/errors
add_compile_options(-Wall -Wextra)
add_compile_options(-pedantic-errors -Werror=pedantic -Werror=main -Wunreachable-code -Wunused -Wunknown-pragmas -Werror=return-type)

# portability
add_compile_options(-Werror=multichar -Werror=address -Werror=sequence-point -Werror=cpp -Werror=strict-aliasing -Werror=strict-null-sentinel)

# extensions
add_compile_options(-Werror=pointer-arith -fno-nonansi-builtins -fstrict-enums -fvisibility-inlines-hidden)

# multiple declaration, shadowing, eval undefined identifier
add_compile_options(-Wshadow -Wundef -Werror=redundant-decls)

add_compile_options(-Werror=ignored-qualifiers)

# class/structs and init
add_compile_options(-Wctor-dtor-privacy -Werror=non-virtual-dtor -Werror=reorder -Werror=uninitialized -Werror=maybe-uninitialized -Werror=delete-non-virtual-dtor)
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
add_compile_options(-Wnonnull -Wcast-qual -Wcast-align -Werror=null-dereference)

# arithmetic/numeric warnings
add_compile_options(-Wfloat-equal -Wsign-compare -Werror=conversion  -Wsign-promo -Werror=shift-overflow -ftrapv)

# logical operations
add_compile_options(-Wlogical-op)

# possible code structure problem
add_compile_options(-Wdisabled-optimization)

# formatting
add_compile_options(-Wformat=2 -Werror=format -Werror=format-security -Werror=format-extra-args)

# exception safety
add_compile_options(-Werror=terminate)

# operations on booleans
if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 6)
	add_compile_options(-Werror=bool-compare)
	if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7)
		add_compile_options(-Werror=bool-operation -Wint-in-bool-context)
	endif()
endif()
# suggestions for improving code
#  inheritance
add_compile_options(-Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override)
# formatting
add_compile_options(-Wsuggest-attribute=format -Wmissing-format-attribute) 

# memory
# write-strings seems to get overritten by pedantic
add_compile_options(-Werror=vla -Werror=array-bounds -Werror=write-strings -Werror=overflow)
if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7)
	add_compile_options(-Werror=alloc-zero -Werror=alloca -Werror=stringop-overflow)
endif()

option(EFFCXX "Enable Effective C++ Warnings (very noisy on correct c++11 code)"  OFF)
if(EFFCXX)
	message(" ===== Enabled Effective C++ Warnings =====")
	add_compile_options(-Weffc++)
endif()


##################################################################
# project structure
add_compile_options(-Wmissing-include-dirs)


##################################################################
# additional debug informations
option(DEBUG_ITERATORS "Additional debug information" OFF)
if(DEBUG_ITERATORS)
    message(" ===== Enabled additional debug information for iterators =====")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
endif()


##################################################################
# sanitizers (checks made at runtime)
# You can choose only one(!)
set(SanitizerValues "NONE;SANADD;SANUNDEF;SANTHREAD" CACHE STRING
	"List of possible values for the sanitizers")
set(SanValue "SANUNDEF" CACHE STRING
	"SanValue chosen by the user at CMake configure time")
set_property(CACHE SanValue PROPERTY STRINGS ${SanitizerValues})

# not using add_compile_options since since flag is also used by the linker
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

option(PROFILE "Enable profiling" OFF)
if(PROFILE)
	message(" ===== Enable profiling ===== ")
	add_compile_options(-pg)
endif()

##################################################################
# mitigations
# abort and provide some useful message instead of ignoring errors, crash at another random place or leave some vulnerability open
option(FORTIFY "Enable fortify sources (already enabled for release target, also enables optimizations)" OFF)
if(FORTIFY)
	message(" ===== Enable fortify sources (always enabled for release target, also enables optimizations) ===== ")
	add_definitions(-D_FORTIFY_SOURCE=2 -O2)
endif()
# FIXME: add check if compiling with -O1, in that case we should use -D_FORTIFY_SOURCE=1...
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2") # =1 when using -O1..

# not using add_compile_options since since flag is also used by the linker
option(RELRO "Enable full relro" OFF)
if(RELRO)
    message(" ===== Enabled full relro =====")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-z,relro,-z,now")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-z,relro")
endif()

option(STACK_PROTECTOR "Enable stack protector on all functions")
if(STACK_PROTECTOR)
    message(" ===== Enabling full stack protector =====")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all")
else()
    if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 5)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector")
    endif()
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-check")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie -fpie -fpic -fPIC")

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	# enable ASLR
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,dynamicbase ")
	# enable DEP
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,nxcompat")
endif()
