#/************************************************************************
# CMake script voor het bouwen van wxWidgets CMake software
# Copyright 2004-2022 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 313 2023-01-30 13:54:35Z ewout $
#************************************************************************/

include_directories(h)

add_compile_definitions(DitJaar=${DitJaar})

set(SRCS filterDesigner.cpp filterDesignerBasis.cpp)

list(SORT SRCS)

if(UNIX)
	if(APPLE)
		set(MACOSX_BUNDLE_INFO_STRING "HAN ESE DSB opdracht4")
		set(OSX_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/platforms/apple/${PROJEKTNAAM}.icns)
		add_executable(${PROJEKTNAAM} MACOSX_BUNDLE ${SRCS} ${OSX_BUNDLE_ICON} ${TALEN})

		ConfigureMacOSXBundlePlist(${PROJEKTNAAM} ${StdCopyrightString} ${OSX_BUNDLE_ICON} ${VersieString})
		ConfigureMacOSXPack(${PROJEKTNAAM} ${OSX_BUNDLE_ICON} "10.7")

		install(TARGETS ${PROJEKTNAAM} RUNTIME DESTINATION bin BUNDLE DESTINATION .)
	else(APPLE)
		add_executable(${PROJEKTNAAM} ${SRCS})
		install(TARGETS ${PROJECTNAAM} RUNTIME DESTINATION bin)
	endif(APPLE)

elseif(WIN32)

	list(APPEND SRCS ${PROJECT_SOURCE_DIR}/platforms/windows/${PROJEKTNAAM}.rc)

	add_executable(${PROJEKTNAAM} WIN32 ${SRCS})
	install(TARGETS ${PROJECTNAAM} RUNTIME DESTINATION bin )
	file(GLOB MSRUNTIME ${GEMDIR}/win32/ms*)
	
	install(TARGETS ${PROJEKTNAAM} RUNTIME DESTINATION bin
	        PERMISSIONS
	        OWNER_WRITE OWNER_READ OWNER_EXECUTE
	        GROUP_READ GROUP_EXECUTE
	        WORLD_READ WORLD_EXECUTE)
endif(UNIX)

include(${CMAKESCRIPTDIR}/FixBundle.cmake)

target_link_libraries(${PROJEKTNAAM} ApplikatieLib FirFilterLib wxBasis ${wxWidgets_LIBRARIES})


PostBuildAktie(${PROJEKTNAAM})
