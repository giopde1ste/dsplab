#/************************************************************************
# CMake basis script voor wxWidgets CMake software bouwen
# Copyright 2004-2021 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: BasisWXWidgets.cmake 4671 2023-02-15 11:40:42Z ewout $
#************************************************************************/

set(BouwDesktopApp ON)

include(${CMAKESCRIPTDIR}/BasisDesktop.cmake)

if(POLICY CMP0077)
	cmake_policy(SET CMP0077 NEW)
endif()

message("HAN ESE wxWidgets software.")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "wxWidgets wordt gebouwd met debugging opties")
	set(wxDEBUGAAN ON)
else()
	message(STATUS "wxWidgets wordt gebouwd met optimalisatie opties")
	set(wxDEBUGAAN OFF)
endif()


# add_definitions(-DWX_PRECOMP)
if (UNIX)

	if (APPLE)

		set(WXVERSIE 3.1) # stand van zaken 15.02.2023 : macports zit op 3.1
		set(wxSTATISCH OFF)

		if (CMAKE_BUILD_TYPE MATCHES Debug)
			#set(WX_CONFIG /opt/local/bin/wx-config)
			set(WX_CONFIG /opt/local/Library/Frameworks/wxWidgets.framework/Versions/wxWidgets/${WXVERSIE}/bin/wx-config)
			#set(WX_CONFIG /usr/local/wxWidgets-${WXVERSIE}/debug/wx-config)
			#set(WX_CONFIG /usr/local/wxWidgets/debug/bin/wx-config)
		else(CMAKE_BUILD_TYPE MATCHES Debug)
			#  set(WX_CONFIG /usr/local/wxwidgets30_release/wx-config)
			#set(WX_CONFIG /usr/local/wxWidgets-${WXVERSIE}/release/wx-config)
			#set(WX_CONFIG /usr/local/wxWidgets/release/bin/wx-config)
			set(WX_CONFIG /opt/local/Library/Frameworks/wxWidgets.framework/Versions/wxWidgets/${WXVERSIE}/bin/wx-config)
		endif(CMAKE_BUILD_TYPE MATCHES Debug)

		# belangrijk om libs exe in bundle te linken naar eigen libs
		set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

	elseif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")

		## wxWidgets wordt niet correct gevonden door CMake
		## omdat wx-config op FreeBSD wxgtk2 ... wordt genoemd
		## Deze patch is niet de eindoplossing maar kan nu worden gebruikt
		## zolang wxWidgets 2.8 als port wordt aangeboden
		set (WXVERSIE 3.2)
		set(wxSTATISCH OFF)
		set(COMP_INFO_TOOL  size)

		message(STATUS "FreeBSD wxWidgets (Release) wordt gebruikt.")
		set(WX_CONFIG "/usr/local/bin/wxgtk3u-${WXVERSIE}-config")
		# additional compiler flags
		set(wxWidgets_CONFIG_EXECUTABLE ${WX_CONFIG})

	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

		if(wxDEBUGAAN)
			set(linuxlibdir debug)
		else()
			set(linuxlibdir release)
		endif()

		if(EXISTS "/usr/local/wxWidgets/${linuxlibdir}/bin/wx-config")
			set(WX_CONFIG "/usr/local/wxWidgets/${linuxlibdir}/bin/wx-config")
		elseif(EXISTS "/usr/local/wxWidgets/release/bin/wx-config")
			set(WX_CONFIG "/usr/local/wxWidgets/release/bin/wx-config")
		elseif(EXISTS "/usr/local/bin/wx-config")
			set(WX_CONFIG "/usr/local/bin/wx-config")
		elseif(EXISTS "/usr/bin/wx-config")
			set(WX_CONFIG "/usr/bin/wx-config")
		else()
			message(FATAL_ERROR "wx-config kon niet worden gevonden.")
		endif(EXISTS "/usr/local/wxWidgets/${linuxlibdir}/bin/wx-config")

		set (WXVERSIE 3.1)
		set(wxSTATISCH OFF)
		set(COMP_INFO_TOOL  size)

		if(CMAKE_BUILD_TYPE MATCHES "Debug")
			set(wxSTATISCH OFF)
		else()
			set(wxSTATISCH ON)
		endif(CMAKE_BUILD_TYPE MATCHES "Debug")

	else(APPLE)
		message(FATAL ERROR "Geen ondersteunde UNIX variant gevonden.")
	endif(APPLE)

	set(wxWidgets_CONFIG_EXECUTABLE ${WX_CONFIG})

elseif(WIN32)

	if (CMAKE_BUILD_TYPE MATCHES "Release")
		message(STATUS "wxWidgets (Static Release) wordt gebruikt.")
		#option(wxWidgets_USE_STATIC ON)
		add_definitions(-DNDEBUG -DwxDEBUG_LEVEL=0)
		set(wxDEBUGAAN OFF)
		set(wxWidgets_CONFIGURATION mswu)
		set(wxSTATISCH ON)
	else()
		message(STATUS "wxWidgets (Static Debug) wordt gebruikt.")
		#option(wxWidgets_USE_STATIC OFF)
		add_definitions(-DwxDEBUG_LEVEL=1)
		set(wxDEBUGAAN ON)
		set(wxWidgets_CONFIGURATION mswud)
		set(wxSTATISCH OFF)
	endif()

	if(NOT MINGW)
		set(wxRootDirTest "C:/wxWidgets")
		# controleer of de github versie aanwezig is
		if(EXISTS ${wxRootDirTest})
			# nieuw versie, gebouwd vanaf source (github)
			# URL =  https://github.com/wxWidgets/wxWidgets/trunk
			message(STATUS "wxWidgets GitHub versie gebruikt.")
			set(wxWidgets_ROOT_DIR ${wxRootDirTest})
		else(EXISTS ${wxRootDirTest})

			set(hv "1" "2" "3")
			set(mv "0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
			set(ov ${mv})

			foreach(hvvar ${hv})
				foreach(mvvar ${mv})
					foreach(ovvar ${ov})
						set(wxVersieTest "${hvvar}.${mvvar}.${ovvar}")
						set(wxRootDirTest "C:/wxWidgets-${wxVersieTest}")
						if(EXISTS ${wxRootDirTest})
							message(STATUS "wxWidgets gevonden op : ${wxRootDirTest}")
							set(wxWidgets_ROOT_DIR ${wxRootDirTest})
							set (WXVERSIE ${wxVersieTest})
						endif(EXISTS ${wxRootDirTest})
					endforeach()
				endforeach()
			endforeach()
		endif(EXISTS ${wxRootDirTest})

	else(NOT MINGW)
		set(wxRootDirTest "C:/Program\ Files\ (x86)\\wxWidgets")
		if(EXISTS ${wxRootDirTest})
			message(STATUS "wxWidgets gevonden op : ${wxRootDirTest}")
			set(wxWidgets_ROOT_DIR ${wxRootDirTest})
			set (WXVERSIE ${wxVersieTest})
		endif(EXISTS ${wxRootDirTest})
	endif(NOT MINGW)

	if(NOT EXISTS ${wxWidgets_ROOT_DIR})
		message(FATAL_ERROR "wxWidgets NIET gevonden.")
	endif(NOT EXISTS ${wxWidgets_ROOT_DIR})

	if(NOT MINGW)
		message(STATUS "Desktop bouw met Microsoft Visual Studio omgeving.")
		
		set(wxBouwLokaalDir ${PROJECT_SOURCE_DIR}/../wxWidgets)
		#include(ExternalProject)
		
		#add_subdirectory(${wxWidgets_ROOT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/wxZelfBouw)
		#ExternalProject_Add(wxWidgets SOURCE_DIR ${wxWidgets_ROOT_DIR} BINARY_DIR ${wxBouwLokaalDir}/bouw INSTALL_DIR ${wxBouwLokaalDir}/lib)
		#ExternalProject_Add_StepDependencies(${PROJEKTNAAM} build wxWidgets)
		
		if(PROJECT_ARCH STREQUAL "x86")
			set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vc_lib")
		else(PROJECT_ARCH STREQUAL "x86")
		
			set(wxMSWBouwLibdir ${wxWidgets_ROOT_DIR}/lib/vc_x64_lib)
			set(wxCMakeBouwLibdir ${wxWidgets_ROOT_DIR}/build/cmake/lib/vc_x64_lib)
			if(EXISTS ${wxCMakeBouwLibdir})
				set(wxWidgets_LIB_DIR ${wxCMakeBouwLibdir})
			elseif(EXISTS ${wxMSWBouwLibdir})
				set(wxWidgets_LIB_DIR ${wxMSWBouwLibdir})
			else()
				message(FATAL_ERROR "de wxWidgets LIB dir werd niet gevonden. Is deze niet gebouwd?")
			endif()	
			
			# msw set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vc_x64_lib")
			#set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/build/cmake/lib/vc_x64_lib")
			#set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/out/build/x64-Debug/lib/vc_x64_dll")
			#set(wxWidgets_LIB_DIR ${wxBouwLokaalDir}/lib)
		endif(PROJECT_ARCH STREQUAL "x86")
	else(NOT MINGW)
		message(STATUS "Desktop bouw met MinGW omgeving.")
		if(PROJECT_ARCH STREQUAL "x86")
			set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/gcc_dll")
		else(PROJECT_ARCH STREQUAL "x86")
			set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/gcc_dll")
		endif(PROJECT_ARCH STREQUAL "x86")
	endif(NOT MINGW)

	if (NOT CUSTOMPAKKETBESCHRIJVING)
		set(SRCS ${SRCS} ${GEMDIR}/win32/dsb.rc)
	endif()

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DwxUSE_DPI_AWARE_MANIFEST=2)
	
endif(UNIX)

#set(wxWidgets_CONFIG_OPTIONS  --version=${WXVERSIE})
message(STATUS "wxWidgets versie   : ${WXVERSIE}")
message(STATUS "wxWidgets Root Dir : ${wxWidgets_ROOT_DIR}")
message(STATUS "wxWidgets statisch : ${wxSTATISCH}")
message(STATUS "wxWidgets debug    : ${wxDEBUGAAN}")

#set(wxWidgets_USE_REL_AND_DBG ON)
set(wxWidgets_USE_STATIC ${wxSTATISCH})
set(wxWidgets_USE_DEBUG ${wxDEBUGAAN})

find_package(wxWidgets COMPONENTS core base adv ${wxWidgetsExtraComponenten} REQUIRED)
if(wxWidgets_FOUND)
	message(STATUS "wxWidgets werd gevonden in (${wxWidgets_ROOT_DIR}).")

	set(wxWidgets_CONFIG_OPTIONS  --version=${WXVERSIE})
	set(wxWidgets_USE_STATIC ${wxSTATISCH})
	set(wxWidgets_USE_DEBUG ${wxDEBUGAAN})

# wxWidgets include (this will do all the magic to configure everything)
	message(STATUS "wxWidgets gevonden. wxWidgets auto config wordt uitgevoerd.")
	include( ${wxWidgets_USE_FILE} )
else()
	message(FATAL_ERROR "wxWidgets werd niet gevonden!")
endif()

include_directories(${GEMDIR}/h ${GEMDIR}/logos)
# add the binary tree to the search path for include files
# that are generated by cmake
include_directories("${PROJECT_BINARY_DIR}/gegenereerd")


#### Installatie spullen

set(INSTALLATIEDIR ${GEMDIR}/installatie)
set(LOGODIR ${GEMDIR}/logos)

set(CPACK_PACKAGE_CONTACT "Ewout Boks (ewout.boks@han.nl)")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSIENUMMER}_${CMAKE_SYSTEM_NAME}_${PROJECT_ARCH}")

if (NOT CUSTOMPAKKETBESCHRIJVING)

	set(CPACK_PACKAGE_NAME ${PROJEKTNAAM})
	set(CPACK_PACKAGE_VENDOR "Hogeschool van Arnhem en Nijmegen")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Dit is een demonstratieversie van de software die bij de opleiding Embedded Systems Engineering van de HAN wordt gebruikt.")
	set(CPACK_PACKAGE_VERSION "${VERSIENUMMER}")
	#set(CPACK_PACKAGE_VERSION_MAJOR "1")
	#set(CPACK_PACKAGE_VERSION_MINOR "0")

	set(CPACK_RESOURCE_FILE_WELCOME ${INSTALLATIEDIR}/${VAK}/welkom.txt)
	set(CPACK_RESOURCE_FILE_README ${INSTALLATIEDIR}/${VAK}/beschrijving.txt)
	set(CPACK_RESOURCE_FILE_LICENSE ${INSTALLATIEDIR}/${VAK}/licentie.txt)
endif(NOT CUSTOMPAKKETBESCHRIJVING)

if(UNIX)

elseif(WIN32)

	file(GLOB NEDERLANDS ${GEMDIR}/installatie/wxstd.mo)

endif(UNIX)





