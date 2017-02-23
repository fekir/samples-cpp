##################################################################
## Settings for the Windows headers

# This settings are not compiler specific, but very often they are set together

# using windows headers: https://msdn.microsoft.com/en-us/library/aa383745%28v=vs.85%29.aspx
add_definitions(-DWIN32_LEAN_AND_MEAN)     # removes rarely used header from windows.h (simply include those headers when needed)
add_definitions(-DNOMINMAX)                # removes min and max macro, use std::min and std::max
add_definitions(-DSTRICT)                  # enable strict mode (adds more typesafety, for exmaple replacing some typedef with structs)

# Copied from windows.h, they may differ from version to version
# Remove functions or defines, unlike WIN32_LEAN_AND_MEAN removing support it's not like a missing header
# Should add option for the most commont variables that may be needed
add_definitions(-DNOGDICAPMASKS)           # CC_*, LC_*, PC_*, CP_*, TC_*, RC_
add_definitions(-DNOVIRTUALKEYCODES)       # VK_*
add_definitions(-DNOWINMESSAGES)           # WM_*, EM_*, LB_*, CB_*
add_definitions(-DNOWINSTYLES)             # WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
add_definitions(-DNOSYSMETRICS)            # SM_*
add_definitions(-DNOMENUS)                 # MF_*
add_definitions(-DNOICONS)                 # IDI_*
add_definitions(-DNOKEYSTATES)             # MK_*
add_definitions(-DNOSYSCOMMANDS)           # SC_*
add_definitions(-DNORASTEROPS)             # Binary and Tertiary raster ops
add_definitions(-DNOSHOWWINDOW)            # SW_*
add_definitions(-DOEMRESOURCE)             # OEM Resource values
add_definitions(-DNOATOM)                  # Atom Manager routines
add_definitions(-DNOCLIPBOARD)             # Clipboard routines
add_definitions(-DNOCOLOR)                 # Screen colors
add_definitions(-DNOCTLMGR)                # Control and Dialog routines
add_definitions(-DNODRAWTEXT)              # DrawText() and DT_*
add_definitions(-DNOGDI)                   # All GDI defines and routines
add_definitions(-DNOKERNEL)                # All KERNEL defines and routines
add_definitions(-DNOMB)                    # MB_* and MessageBox()
add_definitions(-DNOMEMMGR)                # GMEM_*, LMEM_*, GHND, LHND, associated routines
add_definitions(-DNOMETAFILE)              # typedef METAFILEPICT
add_definitions(-DNOOPENFILE)              # OpenFile(), OemToAnsi, AnsiToOem, and OF_*
add_definitions(-DNOSCROLL)                # SB_* and scrolling routines
add_definitions(-DNOSERVICE)               # All Service Controller routines, SERVICE_ equates, etc.
add_definitions(-DNOSOUND)                 # Sound driver routines
add_definitions(-DNOTEXTMETRIC)            # typedef TEXTMETRIC and associated routines
add_definitions(-DNOWH)                    # SetWindowsHook and WH_*
add_definitions(-DNOWINOFFSETS)            # GWL_*, GCL_*, associated routines
add_definitions(-DNOCOMM)                  # COMM driver routines
add_definitions(-DNOKANJI)                 # Kanji support stuff.
add_definitions(-DNOHELP)                  # Help engine interface.
add_definitions(-DNOPROFILER)              # Profiler interface.
add_definitions(-DNODEFERWINDOWPOS)        # DeferWindowPos routines
add_definitions(-DNOMCX)                   # Modem Configuration Extensions
add_definitions(-DNOUSER)                  # All USER defines and routines
add_definitions(-DNONLS)                   # All NLS defines and routines | Code Page Default Values (like CP_UTF8) and MBCS and Unicode Translation (like MB_ERR_INVALID_CHARS)
add_definitions(-DNOMSG)                   # typedef MSG and associated routines

set(WinVerValues "Vista;7;8;8.1;10" CACHE STRING
	"List of possible values for the WinVer cache variable")
set(WinVer "Vista" CACHE STRING
	"WinVer chosen by the user at CMake configure time")

set_property(CACHE WinVer PROPERTY STRINGS ${WinVerValues})

# https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745%28v=vs.85%29.aspx#macros_for_conditional_declarations
# http://doxygen.reactos.org/d9/d16/sdkddkver_8h_source.html
# RC compiler may need numeric value
if("${WinVer}" STREQUAL "Vista")
	# Windows Server 2008, Windows Vista -> 0x0600
	add_definitions(-DNTDDI_VERSION=NTDDI_VISTASP1)
	add_definitions(-DWINVER=_WIN32_WINNT_VISTA)
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
elseif("${WinVer}" STREQUAL "7")
	 # Windows 7
	add_definitions(-DNTDDI_VERSION=NTDDI_WIN7)
	add_definitions(-DWINVER=_WIN32_WINNT_WIN7)
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_WIN7)
elseif("${WinVer}" STREQUAL "8")
	# Windows 8
	add_definitions(-DNTDDI_VERSION=NTDDI_WIN8)
	add_definitions(-DWINVER=_WIN32_WINNT_WIN8)
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_WIN8)
elseif("${WinVer}" STREQUAL "8.1")
	# Windows 8.1
	add_definitions(-DNTDDI_VERSION=NTDDI_WINBLUE)
	add_definitions(-DWINVER=_WIN32_WINNT_WINBLUE)
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_WINBLUE)
elseif("${WinVer}" STREQUAL "10")
	# Windows 10
	add_definitions(-DNTDDI_VERSION=NTDDI_WIN10)
	add_definitions(-DWINVER=_WIN32_WINNT_WIN10)
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_WIN10)
else()
	message( WARNING "Minimal Windows version has not been set" )
endif()

message(STATUS "Minimum Windows version is '${WinVer}'")

##################################################################
# ATL settings
add_definitions(-D_ATL_CSTRING_EXPLICIT_CONSTRUCTORS) # make CString constructor explicit
add_definitions(-D_ATL_ALL_WARNINGS)                  # otherwise ATL may globally disable some warning
add_definitions(-D_SECURE_ATL=1)                      # remove deprecated functions

##################################################################
# MFC settings
add_definitions(-DVC_EXTRALEAN)

##################################################################
# Unicode settings
add_definitions(-D_UNICODE -DUNICODE)

