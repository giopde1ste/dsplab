#/************************************************************************
# CMake initialisatie script voor CMake software bouwen
# Copyright 2004-2022 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: InitDesktop.cmake 4480 2022-06-01 08:52:28Z ewout $
#************************************************************************/

cmake_minimum_required(VERSION 3.10.4)

option(DesktopProjekt "Dit is een desktop projekt" ON)

if(POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif()

if (UNIX)

	if (APPLE)

		message(STATUS "Apple [OS X versie ${CMAKE_SYSTEM_VERSION}] versie wordt gebouwd.")

		# 01.06.2022 Uitgezet ivm Apple M1 (ARM) architectuur ondersteuning
		#if(AppleBouwUniversalBinary)
		#	if (CMAKE_BUILD_TYPE MATCHES "Release")
		#		message(STATUS "OS X release binary wordt uitgerust met i386 en x86_64")
		#		set(CMAKE_OSX_ARCHITECTURES "i386" "x86_64")
		#	else()
		#		message(STATUS "Debug bouw : OS X binary type gezet op x86_64")
		#		set(CMAKE_OSX_ARCHITECTURES "x86_64")
		#	endif (CMAKE_BUILD_TYPE MATCHES "Release")
		#else(AppleBouwUniversalBinary)
		#	message(STATUS "Geen Universal keuze aangegeven : OS X binary type gezet op x86_64")
		#	set(CMAKE_OSX_ARCHITECTURES "x86_64")
		#endif(AppleBouwUniversalBinary)

		#set(SDKVersie "-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")

		# Zie : https://cmake.org/cmake/help/latest/variable/CMAKE_OSX_DEPLOYMENT_TARGET.html
		# Specify the minimum version of the target platform (e.g. macOS or iOS) on which the target binaries are to be deployed.
		# CMake uses this variable value for the -mmacosx-version-min flag or their respective target platform equivalents.
		# For older Xcode versions that shipped multiple macOS SDKs this variable also helps to choose the SDK in case CMAKE_OSX_SYSROOT is unset.
		#If not set explicitly the value is initialized by the MACOSX_DEPLOYMENT_TARGET environment variable, if set,
		# and otherwise computed based on the host platform.
		#The value of this variable should be set prior to the first project() or enable_language() command invocation
		# because it may influence configuration of the toolchain and flags. It is intended to be set locally by the user
		# creating a build tree. This variable should be set as a CACHE entry (or else CMake may remove it while initializing
		# a cache entry of the same name).
		#Despite the OSX part in the variable name(s) they apply also to other SDKs than macOS like iOS, tvOS, or watchOS.
		#This variable is ignored on platforms other than Apple.

		if(NOT MacOSXMinimumVersie)
			set(MacOSXMinimumVersie 10.9)
		endif(NOT MacOSXMinimumVersie)
		set(CMAKE_OSX_DEPLOYMENT_TARGET ${MacOSXMinimumVersie} CACHE STRING "OS X Deployment Target")

	elseif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")

	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

	else(APPLE)
		message(FATAL ERROR "Geen ondersteunde UNIX variant gevonden.")
	endif(APPLE)

endif(UNIX)





