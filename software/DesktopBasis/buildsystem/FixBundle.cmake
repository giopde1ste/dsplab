#/************************************************************************
# CMake basis script voor wxWidgets CMake pakketbouw
# Copyright 2004-2017 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: FixBundle.cmake 4389 2022-02-24 14:42:24Z ewout $
#************************************************************************/

if(POLICY CMP0077)
	cmake_policy(SET CMP0077 NEW)
endif()

include(CPack)

if(APPLE)
	if(NOT AppleInstallatieScriptdir)
		message(FATAL_ERROR "AppleInstallatieScriptdir moet gedefinieerd zijn (in BasisDesktopCMake.cmake).")
	else(NOT AppleInstallatieScriptdir)
		# fixup bundle, copy dynamic libraries into app bundle
		set(APPS "\${CMAKE_INSTALL_PREFIX}/${PROJEKTNAAM}.app")
		# paths to executables
		set(DIRS "\${LIBRARY_OUTPUT_PATH}")   # directories to search for prerequisites
		install(CODE "include (BundleUtilities)
              fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")  " COMPONENT Runtime)

		# fixup bundle, copy dynamic libraries into app bundle
		#set(APPS \${CMAKE_INSTALL_PREFIX}/${PROJEKTNAAM}.app)
		#set(APPS ${PROJECT_SOURCE_DIR}/Executable/${CMAKE_BUILD_TYPE}/${PROJEKTNAAM}.app)

		#message(STATUS "bundle=\"${APPS}\"")
		# paths to executables
		#set(libSearchDirs "\${LIBRARY_OUTPUT_PATH}")   # directories to search for prerequisites
		#list( APPEND libSearchDirs)
		#  ${wxWidgets_LIBRARIES}
		#message(STATUS "packdir=${libSearchDirs}")
		#install(CODE "include(BundleUtilities)
		#              fixup_bundle(\"${APPS}\" \"\" \"${libSearchDirs}\")" COMPONENT Runtime)


		# Voeg een custom target toe om een Apple pkg te bouwen. Dit werkt beter dan het DMG drag and drop bestand.
		set(BundleLokatie ${PROJECT_SOURCE_DIR}/Executable/${CMAKE_BUILD_TYPE}/${PROJEKTNAAM}.app)
		set(PakketNaam ${CMAKE_PROJECT_NAME}_${VERSIENUMMER}_${CMAKE_SYSTEM_NAME}_${PROJECT_ARCH})
		set(PackageNaam ${CMAKE_BINARY_DIR}/${PakketNaam}.pkg)
		set(ProduktNaam ${CMAKE_BINARY_DIR}/${PakketNaam}_prod.pkg)
		set(DistributieXML ${CMAKE_BINARY_DIR}/${PakketNaam}.xml)
		set(PkgInstallatieDir "/Applications")
		set(KwikPkg ${AppleInstallatieScriptdir}/quickpkg/quickpkg)

		add_custom_command(OUTPUT MaakPackageTool
		                   # Voer eerst make install uit op de applikatie te vullen met alle benodigde libs en bestanden
		                   COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install --
		                   COMMAND ${KwikPkg} ${BundleLokatie} --output ${PackageNaam} #
		                   COMMAND ${CMAKE_COMMAND} -E echo ""
		                   COMMAND ${CMAKE_COMMAND} -E echo "======================================================="
		                   COMMAND ${CMAKE_COMMAND} -E echo "Klaar! De Apple Installer package \"${PackageNaam}\" is gemaakt."
		                   COMMENT "Een Apple installer package wordt aangemaakt.................")

		add_custom_command(OUTPUT MaakProduktTool
		                   DEPENDS ${PackageNaam}
		                   COMMAND productbuild --synthesize --package ${PackageNaam} ${DistributieXML}
		                   COMMAND productbuild --distribution ${DistributieXML} --package-path ${PackageNaam} ${ProduktNaam}
		                   COMMENT "Apple produkt \"${ProduktNaam}\" gemaakt.")
		#CMD = productbuild --distribution ${DistributieXML} --package-path ${PackageNaam} ${ProduktNaam}")

		add_custom_command(OUTPUT MaakPackageToolOud
		                   COMMAND pkgbuild --component ${BundleLokatie} --install-location ${PkgInstallatieDir} ${PackageNaam}
		                   COMMENT "Apple pkg installer \"${PackageNaam}\" gemaakt.")

		add_custom_command(OUTPUT MaakProduktToolOud
		                   COMMAND productbuild --component ${BundleLokatie} ${PkgInstallatieDir} ${PackageNaam} #
		                   COMMENT "Apple pkg installer \"${PackageNaam}\" gemaakt.")


		add_custom_target(MaakPackage
		                  DEPENDS MaakPackageTool)

		add_custom_target(MaakProdukt
		                  DEPENDS MaakProduktTool)


	endif(NOT AppleInstallatieScriptdir)


elseif(WIN32)

elseif(UNIX)

	if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

	endif()

endif(APPLE)

include (InstallRequiredSystemLibraries)