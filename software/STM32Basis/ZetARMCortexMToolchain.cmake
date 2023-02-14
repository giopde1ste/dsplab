## Copyright 2011-2019 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: ZetARMCortexMToolchain.cmake 4634 2022-11-10 16:11:28Z ewout $

message(STATUS "ARM Cortex M architectuur toolchain selektie.")

option(ARMCORTEXM "Bouw voor ARM Cortex-M architectuur" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS Off)

# top software dir is hierboven. met ook de externals
if(UNIX)
	message(STATUS "Unix platform gedetekteerd.")
	if(NOT EXTERNALDIR)
		set(EXTERNALDIR $ENV{HOME}/techniek/swexternals)
	endif(NOT EXTERNALDIR)
	if(APPLE)
		# 18.11.2021 Dit is cruciaal om cross compile te veroorzaken en op Apple de -isysroot = .... sdk opties te vermijden.
		set(CMAKE_SYSTEM_NAME "Generic")

		if(NOT OverrideGCC_G++)
			option(OverrideGCC_G++ "Gebruik de C++ compiler voor C" ON)
		endif(NOT OverrideGCC_G++)
		#set(CMAKE_TOOLCHAIN_FILE "/Users/kiwanda/techniek/swexternals/buildsystem/cmake/embedded/arm/ARMCortexMToolchain.cmake")
		set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/ARMCortexMToolchain.cmake")
		set(ToolchainOntdekt ON)
	endif(APPLE)
elseif(WIN32)
	message(STATUS "Windows platform gedetekteerd.")
	if(NOT EXTERNALDIR)
		# Windows volgt niet unix symlinks
		set(EXTERNALDIR "../../swexternals")
	endif(NOT EXTERNALDIR)
else(UNIX)
	message(FATAL_ERROR "Geen bruikbaar platform gedetekteerd!")
	#set(EXTERNALDIR $ENV{HOME}/techniek/swexternals)
endif(UNIX)

set(BUILDSYSTEMDIR ${EXTERNALDIR}/buildsystem/cmake/embedded)

option(GebruikTargetLibs "Gebruik Target Libs" OFF)
