#/************************************************************************
# CMake basis script voor CMake software bouwen
# Copyright 2004-2022 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: BasisDesktop.cmake 4680 2023-03-08 14:00:01Z ewout $
#************************************************************************/

string(TIMESTAMP DitJaar "%Y")
string(TIMESTAMP CopyrightDatum "%d.%m.%Y")
set(StdCopyrightString "(c)2006-${DitJaar} ir drs E.J Boks\nOpleiding Embedded Systems Engineering\nHogeschool van Arnhem en Nijmegen")
set(SchoolCopyright "(c)2006-${DitJaar} ir drs E.J Boks\\nOpleiding Embedded Systems Engineering\\nHogeschool van Arnhem en Nijmegen")

message(${StdCopyrightString})
# message(${StdCopyrightString})

if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0040)
	cmake_policy(SET CMP0040 NEW)
endif()

if(POLICY CMP0026)
	cmake_policy(SET CMP0026 NEW)
endif()

if(POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif()

if(POLICY CMP0077)
	cmake_policy(SET CMP0077 NEW)
endif()

if (PROJECTNAAM)
	message(WARNING "Gebruikt PROJEKTNAAM i.p.v PROJECTNAAM !")
	set(PROJECTNAAM PROJEKTNAAM)
endif(PROJECTNAAM)

if (NOT VERSIEHOOFDNUMMER)
	message(STATUS "Versiehoofdnummer niet gezet. Versiehoofdnummer is nu 1")
	set(VERSIEHOOFDNUMMER 1)
endif(NOT VERSIEHOOFDNUMMER)

if (NOT VERSIEONDERNUMMER)
	message(STATUS "Versieondernummer niet gezet. Versieondernummer is nu 0")
	set(VERSIEONDERNUMMER 0)
endif(NOT VERSIEONDERNUMMER)

if (NOT VERSIESUBONDERNUMMER)
	message(STATUS "Versiesubondernummer niet gezet. Versiesubondernummer is nu 0")
	set(VERSIESUBONDERNUMMER 0)
endif(NOT VERSIESUBONDERNUMMER)

if (NOT VERSIESUBONDERLETTER)
	message(STATUS "Versiesubonderletter niet gezet. Versiesubonderletter is nu a")
	set(VERSIESUBONDERLETTER a)
endif(NOT VERSIESUBONDERLETTER)

set(VERSIENUMMER ${VERSIEHOOFDNUMMER}.${VERSIEONDERNUMMER}.${VERSIESUBONDERNUMMER})

# Location where cmake first looks for cmake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKESCRIPTDIR})

add_compile_definitions(BouwDesktopApp)

message("\nSysteeminspektie : pointer grootte is : ${CMAKE_SIZEOF_VOID_P} bytes.")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "64 bit architectuur gedetecteerd.")
	set( PROJECT_ARCH "x86_64" )
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "32 bit architectuur gedetecteerd.")
	set( PROJECT_ARCH "x86" )
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

# geen ccache gebruik in deze omstandigheden 
if($ENV{CLION_IDE})
	set(CCACHEGEBRUIKT OFF)
endif($ENV{CLION_IDE})

set(CLionIdeGebruikt $ENV{CLION_IDE})
if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release;SizeRelease;DebugRelease")
    endif()
endif()

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE)
  set(default_build_type "Debug")
  message(WARNING "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  if(NOT CMAKE_CONFIGURATION_TYPES)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "SizeRelease" "DebugRelease")
  endif(NOT CMAKE_CONFIGURATION_TYPES)	
endif()

# Standaard staat de dir op Executable, maar dit kan worden aangepast.
if(NOT ExecutableDir)
	set(ExecutableDir Executable)
else(NOT ExecutableDir)
	message(STATUS "Custom executable dir gezet : ${ExecutableDir}")
endif(NOT ExecutableDir)

if(NOT WIN32)
  set(UitvoerDir ${PROJECT_SOURCE_DIR}/${ExecutableDir}/${CMAKE_BUILD_TYPE})
  set(InstallatieDir ${UitvoerDir})
  
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UitvoerDir} CACHE STRING "" FORCE)
else(NOT WIN32)
  # bij make install wordt alles in de Executable dir gezet
  if(BouwDesktopApp)
    
    set(UitvoerDir ${PROJECT_SOURCE_DIR}/${ExecutableDir}/${PROJECT_ARCH})
    set(InstallatieDir ${UitvoerDir}/${CMAKE_BUILD_TYPE}/)
    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UitvoerDir} CACHE STRING "" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${UitvoerDir}/Debug/bin CACHE STRING "" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${UitvoerDir}/Release/bin CACHE STRING "" FORCE)		
  else(BouwDesktopApp)
    set(UitvoerDir ${PROJECT_SOURCE_DIR}/${ExecutableDir}/${CMAKE_BUILD_TYPE})
    set(InstallatieDir ${UitvoerDir})
  endif(BouwDesktopApp)
endif(NOT WIN32)

mark_as_advanced(UitvoerDir InstallatieDir)

set(CMAKE_INSTALL_PREFIX ${InstallatieDir} CACHE STRING "" FORCE)
file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)
#message(STATUS "[Debug] Install = ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${UitvoerDir} CACHE STRING "" FORCE)

message(STATUS "Uitvoer dir is : ${UitvoerDir}")
message(STATUS "Installatie dir is : ${InstallatieDir}")
######

if (UNIX)
	message(STATUS "Unix wordt gebouwd.")
	message(STATUS "Gedetecteerd OS : ${CMAKE_SYSTEM_NAME}")

	# standaard C en CPP vlaggen voor Desktop Unix
	set(CMAKE_StdCompileVlaggen "-pipe -funsigned-char -fno-strict-aliasing -fstack-protector")
	set(CMAKE_WaarschuwingsVlaggen "-Wall -Wpointer-arith -Wswitch -Wreturn-type -Wshadow -Wunused -Werror -Wno-deprecated-declarations -Wredundant-decls")
	#-Wsign-promo")
	#  -Wunused-parameter -Wredundant-decls
	set(CMAKE_C_BASISVLAGGEN "${CMAKE_StdCompileVlaggen}  ${CMAKE_WaarschuwingsVlaggen}")
	set(CMAKE_CXX_BASISVLAGGEN "${CMAKE_StdCompileVlaggen} -std=c++14  ${CMAKE_WaarschuwingsVlaggen} -Wno-ctor-dtor-privacy -Woverloaded-virtual -Wno-parentheses -Wno-variadic-macros -Wshadow -Wno-inconsistent-missing-override")

	if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CMAKE_C_BASISVLAGGEN "${CMAKE_C_BASISVLAGGEN} -ffunction-sections -fdata-sections")
		set(CMAKE_CXX_BASISVLAGGEN "${CMAKE_CXX_BASISVLAGGEN} -ffunction-sections -fdata-sections")
	endif(NOT CMAKE_BUILD_TYPE MATCHES "Debug")

	add_definitions(-D_BSD_SOURCE)

	set(GebruikerNaam $ENV{USER})

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(ONTVLOOI ON)
		message(STATUS "Software wordt gebouwd met debugging opties")
		add_definitions(-ggdb -O0)
	else()
		set(ONTVLOOI OFF)
		message(STATUS "Software wordt gebouwd met optimalisatie opties")
		add_definitions(-O2 -DNDEBUG)
	endif()

	find_package(cppcheck QUIET)
	find_package(Clang REQUIRED)

	set(ccompiler clang)
	set(cppcompiler clang++)
	set(COMP_INFO_TOOL size)

	if(CPPCHECK_FOUND)
		message(STATUS "CPPCheck tool aanwezig op dit systeem")

		# de macro ondersteunt maximaal 3 extra argumenten
		macro(CppCheckRun NAAM SOURCEDIR DESTDIR)

			add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/${NAAM}.xml
			                   POST_BUILD
			                   DEPENDS ${SOURCEDIR}
			                   COMMAND ${CPPCHECK_EXECUTABLE} --enable=style,information --inline-suppr --xml-version=2 ARGS "${ARGV3}" "${ARGV4}" "${ARGV5}" "${SOURCEDIR}" 2> "${PROJECT_BINARY_DIR}/${NAAM}.xml"
			                   COMMAND ${CPPCHECK_EXECUTABLE}-htmlreport ARGS "--file=${PROJECT_BINARY_DIR}/${NAAM}.xml" "--report-dir=${DESTDIR}" "--source-dir=${SOURCEDIR}" 2>&1 /dev/null
			                   #		  COMMAND rm "${PROJECT_BINARY_DIR}/${NAAM}.xml"
			                   COMMENT "Bouw een CppCheck rapport voor ${NAAM}")

			add_custom_target(check${NAAM} ALL
			                  DEPENDS ${PROJECT_BINARY_DIR}/${NAAM}.xml)

		endmacro()

	endif(CPPCHECK_FOUND)

	if(CPPCHECKAAN AND CPPCHECK_FOUND)
		set(ARGV4 "-i${PROJECT_SOURCE_DIR}/debug")
		set(ARGV5 "-i${PROJECT_SOURCE_DIR}/release")
		set(ARGV6 "-i${PROJECT_SOURCE_DIR}/XCode")
		CppCheckRun(${PROJEKTNAAM}Test ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/${PROJEKTNAAM}_testresultaten ${ARGV4} ${ARGV5} ${ARGV6})
	endif(CPPCHECKAAN AND CPPCHECK_FOUND)

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
		message(STATUS "Mac OS X minimum versie gezet : ${MacOSXMinimumVersie}")
		set(CMAKE_C_FLAGS "${CMAKE_C_BASISVLAGGEN}" )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_BASISVLAGGEN} -stdlib=libc++")
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINKBASISVLAGGEN}")

		if(XCODE_VERSION)
			message(STATUS "Apple XCode ${XCODE_VERSION} wordt gebruikt.")
		else(XCODE_VERSION)
			message(STATUS "Apple Terminal wordt gebruikt.")

			if(CCACHEGEBRUIKT AND (${CMAKE_SYSTEM_VERSION} LESS 13.0.0))
				message(STATUS "CCache gebruik gevraagd.")
				find_package(CCache)
				if (CCACHE_LIBDIR)
					set(CMAKE_C_COMPILER ${CCACHE_LIBDIR}/cc)
					set(CMAKE_CXX_COMPILER ${CCACHE_LIBDIR}/c++)
				endif (CCACHE_LIBDIR)
			endif()
		endif(XCODE_VERSION)

		# belangrijk om libs exe in bundle te linken naar eigen libs
		set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

		add_definitions(-DGeenIkoon)

		#-------------------------------------------------------------------------------
		# This macro will set all the variables necessary to have a "good" OS X Application
		# bundle. The variables are as follows:
		#  PROJECT_NAME - which can be taken from the ${PROJECT_NAME} variable is needed
		#  ICON_FILE_PATH - The complete path to the bundle icon file
		#  VERSION_STRING - The version string that you wish to use for the bundle. For OS X
		#   this string is usually XXXX.YY.ZZ in type. Look at the Apple docs for more info
		#-------------------------------------------------------------------------------
		macro(ConfigureMacOSXBundlePlist PROJECT_NAME COPYRIGHT_STRING ICON_FILE_PATH VERSION_STRING)
			# message(STATUS "ConfigureMacOSXBundlePlist for ${PROJECT_NAME} ")
			IF(CMAKE_BUILD_TYPE MATCHES "Release")
				set(DBG_EXTENSION "")
			else()
				set(DBG_EXTENSION ${DEBUG_EXTENSION})
			endif()
			get_filename_component(ICON_FILE_NAME "${ICON_FILE_PATH}" NAME)

			#CFBundleGetInfoString
			set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} Version ${VERSION_STRING}, ${COPYRIGHT_STRING}")
			set(MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME})
			set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}")
			#CFBundleLongVersionString
			set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} Version ${VERSION_STRING}")
			set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}${DBG_EXTENSION})
			set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING})
			set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING})
			set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT_STRING})

			# kopieer het sjabloon met de ingevulde velden als plist voor deze applikatie.
			# belangrijk : NSPrincipalClass MOET anders geen retina applikatie maar een vage verschijning !
			set(AppleInstallatieScriptdir ${CMAKESCRIPTDIR}/installatie/Apple)
			set(PListSjabloon ${AppleInstallatieScriptdir}/MacOSXBundleInfo.plist.in)
			set(PListBestand "${PROJECT_BINARY_DIR}/gegenereerd/Info.plist")
			configure_file (${PListSjabloon} ${PListBestand})

			set(PListBestand "${PROJECT_BINARY_DIR}/gegenereerd/Info.plist")
			configure_file (${PListSjabloon} ${PListBestand})

			set_target_properties(${PROJEKTNAAM} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PListBestand})

			set(${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} ${ICON_FILE_PATH})

			set_source_files_properties(${ICON_FILE_NAME} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
			# let op deze is verschillend (path ipv name)
			set_source_files_properties(${ICON_FILE_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		endmacro()

		# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
		# use, i.e. don't skip the full RPATH for the build tree
		set(CMAKE_SKIP_BUILD_RPATH  FALSE)

		# when building, don't use the install RPATH already
		# (but later on when installing)
		set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

		set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

		# add the automatically determined parts of the RPATH
		# which point to directories outside the build tree to the install RPATH
		set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

		# the RPATH to be used when installing, but only if it's not a system directory
		LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
		IF("${isSystemDir}" STREQUAL "-1")
			set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
		ENDIF("${isSystemDir}" STREQUAL "-1")

		add_definitions(-DGeenIkoon)

	elseif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")

		message(STATUS "FreeBSD versie wordt gebouwd.")

		if(CCACHEGEBRUIKT)
			message(STATUS "clang ccache links worden gebruikt.")
			set(TOOLCHAINDIR /usr/local/libexec/ccache)
			set(CMAKE_C_COMPILER ${TOOLCHAINDIR}/clang)
			set(CMAKE_CXX_COMPILER ${TOOLCHAINDIR}/clang++)
		endif(CCACHEGEBRUIKT)

		set(CMAKE_C_FLAGS "${CMAKE_C_BASISVLAGGEN}" )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_BASISVLAGGEN}")
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINKBASISVLAGGEN}")

		# strip de binary !
		list(APPEND CMAKE_CXX_LINK_FLAGS " -Wl,--gc-sections")

	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

		message(STATUS "SUN Solaris/Linux versie wordt gebouwd.")

		set(COMP_INFO_TOOL  size)
		add_definitions(-D_DEFAULT_SOURCE)
		if(CCACHEGEBRUIKT)
			message(STATUS "CLang ccache links worden gebruikt.")
			set(TOOLCHAINDIR /usr/lib/ccache)
			set(CMAKE_C_COMPILER ${TOOLCHAINDIR}/${ccompiler})
			set(CMAKE_CXX_COMPILER ${TOOLCHAINDIR}/${cppcompiler})
		else(CCACHEGEBRUIKT)
			#      set(CMAKE_C_COMPILER ${ccompiler})
			#      set(CMAKE_CXX_COMPILER ${cppcompiler})
		endif(CCACHEGEBRUIKT)

		set(CMAKE_C_FLAGS "${CMAKE_C_BASISVLAGGEN}" )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_BASISVLAGGEN}")
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINKBASISVLAGGEN}")

		# strip de binary !
		list(APPEND CMAKE_CXX_LINK_FLAGS "-Wl,--gc-sections")

		#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)

	else(APPLE)
		message(FATAL ERROR "Geen ondersteunde UNIX variant gevonden.")
	endif(APPLE)

	# Analyse spullen onder Unix systemen
	find_package(PythonInterp)
	find_package(Valgrind)
	if (VALGRIND_FOUND)
		if (PYTHONINTERP_FOUND)

			enable_testing()

			# Valgrind Software testing raamwerk
			#MEMORYCHECK_COMMAND:FILEPATH=${Valgrind_Executable}
			#set(CTEST_MEMORYCHECK_COMMAND ${Valgrind_Executable} )
			#set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
			#add_test(NAME test
			#        WORKING_DIRECTORY ${my_outputdirectory}
			#        COMMAND test_exe)
			#   ctest -D ExperimentalBuild
			#   ctest -R test -D ExperimentalMemChec

			# gebruik Python script
			add_test(memtest ${CMAKESCRIPTDIR}/memtest.py ${CMAKE_CURRENT_BINARY_DIR}/${PROJEKTNAAM} ${CMAKE_BINARY_DIR})

		else(PYTHONINTERP_FOUND)
			message(STATUS "Python ontbreekt. Hierdoor is het niet mogelijk een memory test uit te voeren")
			message(STATUS "Software ontwikkeling wordt hier echter niet onmogelijk door gemaakt.")
		endif(PYTHONINTERP_FOUND)

	else(VALGRIND_FOUND)
		message(STATUS "Valgrind ontbreekt. Hierdoor is het niet mogelijk een memory test uit te voeren")
		message(STATUS "Software ontwikkeling wordt hier echter niet onmogelijk door gemaakt.")
	endif(VALGRIND_FOUND)

elseif(WIN32)

	message(STATUS "Microsoft Windows wordt gebouwd.")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DMSWindows)

	set(GebruikerNaam $ENV{USERNAME})

	if(MINGW)
		# standaard C en CPP vlaggen voor Desktop Unix
		set(CMAKE_StdCompileVlaggen "-pipe -funsigned-char -fno-strict-aliasing -fstack-protector")
		set(CMAKE_WaarschuwingsVlaggen "-Wall -Wpointer-arith -Wswitch -Wreturn-type -Wshadow -Wunused -Werror -Wno-deprecated-declarations -Wredundant-decls")
		#-Wsign-promo")
		#  -Wunused-parameter -Wredundant-decls
		set(CMAKE_C_BASISVLAGGEN "${CMAKE_StdCompileVlaggen}  ${CMAKE_WaarschuwingsVlaggen}")
		set(CMAKE_CXX_BASISVLAGGEN "${CMAKE_StdCompileVlaggen} -std=c++14  ${CMAKE_WaarschuwingsVlaggen} -Wno-ctor-dtor-privacy -Woverloaded-virtual -Wno-parentheses -Wno-variadic-macros -Wshadow")

		if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
			set(CMAKE_C_BASISVLAGGEN "${CMAKE_C_BASISVLAGGEN} -ffunction-sections -fdata-sections")
			set(CMAKE_CXX_BASISVLAGGEN "${CMAKE_CXX_BASISVLAGGEN} -ffunction-sections -fdata-sections")
		endif(NOT CMAKE_BUILD_TYPE MATCHES "Debug")

		set(CMAKE_C_FLAGS "${CMAKE_C_BASISVLAGGEN}" )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_BASISVLAGGEN}")
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINKBASISVLAGGEN}")
	endif(MINGW)

	if(POLICY CMP0026)
		cmake_policy(SET CMP0026 NEW)
	endif()

	macro(DLLKopieerBestanden DLLBestanden DLL_DIR)
		foreach(dllBestand ${DLLBestanden})
			get_filename_component(dllBestand ${dllBestand} NAME)
			string(REPLACE ".dll" "" dllBestand ${dllBestand})
			set(Bron "${DLL_DIR}/${dllBestand}.dll")
			message(STATUS "${dllBestand} uit ${DLL_DIR} wordt nu naar Bouwdir gekopieerd")
			#     get_target_property(EXE_LOC ${PROJEKTNAAM} LOCATION)
			set(Bestemming $<TARGET_FILE_DIR:${PROJEKTNAAM}>)
			#message(STATUS,"${Bron} wordt nu naar Bouwdir gekopieerd")
			add_custom_command(TARGET ${PROJEKTNAAM} POST_BUILD        # Adds a post-build event to MyTest
			                   COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
			                   ${Bron} 	          # <--this is in-file
			                   ${Bestemming})     # <--this is out-file path

		endforeach()
	endmacro()

	macro(DLLKopieerBestand dllBestand)
		message(STATUS "${dllBestand} wordt nu naar Bouwdir gekopieerd")
		# get_filename_component(dllBestandNaam ${dllBestand} NAME)

		#     get_target_property(EXE_LOC ${PROJEKTNAAM} LOCATION)
		set(Bestemming $<TARGET_FILE_DIR:${PROJEKTNAAM}>)
		#message(STATUS,"${Bron} wordt nu naar Bouwdir gekopieerd")
		add_custom_command(TARGET ${PROJEKTNAAM} POST_BUILD        # Adds a post-build event to MyTest
		                   COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
		                   ${dllBestand} 	          # <--this is in-file
		                   ${Bestemming})     # <--this is out-file path	
	endmacro()

	if (NOT CUSTOMPAKKETBESCHRIJVING)
		set(SRCS ${SRCS} ${GEMDIR}/win32/dsb.rc)
	endif()

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(UNIX)

message(STATUS "Naam van de ontwikkelaar : ${GebruikerNaam}")

include_directories(${GEMDIR}/h ${GEMDIR}/logos)
# Voeg de lokatie van de eigen gegenereerde bestanden toe aan het zoekpad
include_directories("${PROJECT_BINARY_DIR}/gegenereerd")

function(MaakLib Naam Bronnen)

	# MaakLijst("${Bronnen}")
	# maak de library
	add_library(${Naam} STATIC "${Bronnen}")

endfunction(MaakLib Naam Bronnen)

# voor info na het bouwen
function(PostBuildAktie UitvoerbaarBestand)

	if(UNIX)
		if (InterfaceTaal STREQUAL "Nederlands")
			set(Kommentaar "De software is met succes gecompileerd. Dit is \"${PROJEKTNAAM}\".")
		else(InterfaceTaal STREQUAL "Nederlands")
			set(Kommentaar "The software was compiled successfully. This is \"${PROJEKTNAAM}\".")
		endif(InterfaceTaal STREQUAL "Nederlands")

		# plak eventueel .elf achter de bestandsnaam

		if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
			set(outputdir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		else(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
			set(outputdir ${CMAKE_CURRENT_BINARY_DIR})
		endif(CMAKE_RUNTIME_OUTPUT_DIRECTORY)


		#message(STATUS "info = ${COMP_INFO_TOOL}")
		if(APPLE)
			set(goedenaam ${outputdir}/${UitvoerbaarBestand}.app/Contents/MacOS/${UitvoerbaarBestand})

			# voeg bouw info to aan de output
			add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
			                   COMMAND ${COMP_INFO_TOOL} -x ${goedenaam}
			                   DEPENDS ${goedenaam}
			                   COMMENT "Mac OS X post-aktie:\n${Kommentaar}")
		else(APPLE)
			set(goedenaam ${outputdir}/${UitvoerbaarBestand})

			add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
			                   COMMAND ${COMP_INFO_TOOL} -A -x ${goedenaam}
			                   DEPENDS ${oudenaam}
			                   COMMENT "FreeBSD post-aktie:\n${Kommentaar}")
		endif(APPLE)

	endif(UNIX)
endfunction()

#### Installatie spullen 
set(INSTALLATIEDIR ${GEMDIR}/installatie)
set(LOGODIR ${GEMDIR}/logos)

set(CPACK_PACKAGE_CONTACT "Ewout Boks (ewout.boks@han.nl)")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSIENUMMER}_${CMAKE_SYSTEM_NAME}_${PROJECT_ARCH}")

if (NOT CUSTOMPAKKETBESCHRIJVING)
	message(STATUS "(Voor CPack) Een standaard pakketbeschrijving wordt bijgevoegd.")
	set(CPACK_PACKAGE_NAME ${PROJEKTNAAM})
	set(CPACK_PACKAGE_VENDOR "Hogeschool van Arnhem en Nijmegen")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Dit is een demonstratieversie van de software die bij de opleiding Embedded Systems Engineering van de HAN wordt gebruikt.")
	set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSIENUMMER}_${CMAKE_SYSTEM_NAME}_${PROJECT_ARCH}")
	set(CPACK_PACKAGE_VERSION_MAJOR "${VERSIEHOOFDNUMMER}")
	set(CPACK_PACKAGE_VERSION_MINOR "${VERSIEONDERNUMMER}")
	set(CMAKE_PACKAGE_PATCH_VERSION "0")
	set(CPACK_PACKAGE_VERSION "${VERSIENUMMER}")

	set(CPACK_RESOURCE_FILE_WELCOME ${INSTALLATIEDIR}/${VAK}/welkom.txt)
	set(CPACK_RESOURCE_FILE_README ${INSTALLATIEDIR}/${VAK}/beschrijving.txt)
	set(CPACK_RESOURCE_FILE_LICENSE ${INSTALLATIEDIR}/${VAK}/licentie.txt)
endif(NOT CUSTOMPAKKETBESCHRIJVING)

if(UNIX)

	if(APPLE)

		macro(ConfigureMacOSXPack PROJECT_NAME ICON_FILE_PATH OSX_MINVERSION)

			set(CPACK_OSX_PACKAGE_VERSION ${OSX_MINVERSION})
			set(CPACK_PACKAGE_ICON ${ICON_FILE_PATH})
			set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
			#set(CPACK_OSX_BUNDLE_ICON ${GEMDIR}/apple/${VAK}.icns)
		endmacro()

		set(CPACK_BINARY_DRAGNDROP ON)
		set(CPACK_BINARY_PRODUCTBUILD ON)
		set(CPACK_SOURCE_TZ OFF)
		set(CPACK_SOURCE_TGZ OFF)
		set(CPACK_BINARY_TGZ OFF)
		set(CPACK_BINARY_STGZ OFF)
		set(CPACK_PACKAGING_INSTALL_PREFIX "/")
		set(CPACK_PACKAGE_DEFAULT_LOCATION "/Applications")


	endif(APPLE)

	if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
		add_definitions(-D_FORTIFY_SOURCE=2)
		set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")

		set(CPACK_BINARY_DEB ON)
		set(CPACK_BINARY_STGZ ON)
		set(CPACK_BINARY_TGZ OFF)
		set(CPACK_BINARY_TZ OFF)

		#set (CPACK_set_DESTDIR "on")
		set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
		set (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
		set (CPACK_DEBIAN_PACKAGE_SECTION "development")
		set (CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
		set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ewout Boks <ewout.boks@han.nl>")
		set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.han-ese.nl")
		set(CPACK_COMPONENTS_ALL Libraries ApplicationData)
		# Auto opzoeken van alle dependencies
		set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
	endif()

elseif(WIN32)

	set(CMAKE_NSIS_MUI_ICON ${GEM_DIR}/win32\\\\hanlogo.ico)
	set(CMAKE_NSIS_MUI_UNICON ${CMAKE_NSIS_MUI_ICON})
	set(CPACK_CREATE_DESKTOP_LINKS ${PROJEKTNAAM})
	set(CPACK_NSIS_MENU_LINKS "https://ese.han.nl" "HAN Embedded Systems Engineering")
	set(CPACK_PACKAGE_EXECUTABLES ${PROJEKTNAAM} ${PROJEKTNAAM})

	set(CPACK_PACKAGE_ICON
	    "${LOGODIR}\\\\hanlogo.png")

	set(CPACK_NSIS_MUI_FINISHPAGE_RUN ${PROJEKTNAAM})

	if(PROJECT_ARCH STREQUAL "x86_64")
		set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
	endif()

	# Waarschuw niet (voor studenten met de VS2010 Express versie op hun systeem)
	# 14.01.2021 VEROUDERD IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
	#	set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
	#	message(STATUS "vcruntime = ${VCRuntimeLibs} ${VCRuntime2Libs}")
	# ENDIF()


endif(UNIX)





