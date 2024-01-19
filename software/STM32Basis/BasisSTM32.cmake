	## Copyright 2011-2022 Hogeschool van Arnhem en Nijmegen
	## Opleiding Embedded Systems Engineering
	## Ewout Boks
	## $Id: BasisSTM32.cmake 4648 2022-12-08 16:06:11Z ewout $

	string(TIMESTAMP DitJaar "%Y")

	option(ARMCORTEXM "Ontwikkel voor ARM Cortex-M" ON)
	
	if (PROJECTNAAM)
		message(WARNING "Gebruikt PROJEKTNAAM i.p.v PROJECTNAAM !")
		set(PROJECTNAAM PROJEKTNAAM)
	endif(PROJECTNAAM)

	#OPTION(ONTVLOOI "Bouw het bestand met debug informatie" OFF)
	# Set a default build type if none was specified
	if(NOT CMAKE_BUILD_TYPE) # AND NOT CMAKE_CONFIGURATION_TYPES)
		message(STATUS "Setting build type to 'Debug' as none was specified.")
		set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
		# Set the possible values of build type for cmake-gui
		set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
	else(NOT CMAKE_BUILD_TYPE) #  AND NOT CMAKE_CONFIGURATION_TYPES)

		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			option(ONTVLOOI "Bouw kode met debugging symbolen" ON)
			message(STATUS "Software wordt gebouwd met debugging opties")
			add_definitions(-O0 -g -fno-default-inline)
		elseif(CMAKE_BUILD_TYPE STREQUAL "DebugRelease")
			option(ONTVLOOI "Bouw kode met debugging symbolen" ON)
			message(STATUS "Software wordt gebouwd met debug optimalisatie opties")
			add_definitions(-Og -g -fno-default-inline)
		elseif(CMAKE_BUILD_TYPE STREQUAL "SizeRelease")
			option(ONTVLOOI "Bouw kode met debugging symbolen" OFF)
			message(STATUS "Software wordt gebouwd met optimalisatie opties")
			add_definitions(-Os -DNDEBUG)
		else()
			set(CMAKE_BUILD_TYPE Release CACHE STRING "Release build" FORCE)
			option(ONTVLOOI "Bouw kode met debugging symbolen"  OFF)
			message(STATUS "Software wordt gebouwd met Release optimalisatie opties")
			add_definitions(-O2 -DNDEBUG)
		endif()

	endif(NOT CMAKE_BUILD_TYPE) #  AND NOT CMAKE_CONFIGURATION_TYPES)

	ENABLE_LANGUAGE (ASM-ATT OPTIONAL)

	set (CMAKE_FIND_ROOT_PATH_MODE NEVER)

	if(ONTVLOOI)
		message(STATUS "StdPeriph/HAL bibliotheek USE_FULL_ASSERT staat AAN")
		add_definitions(-DUSE_FULL_ASSERT)
	else(ONTVLOOI)
		message(STATUS "StdPeriph/HAL bibliotheek USE_FULL_ASSERT staat UIT")
	endif(ONTVLOOI)

	# Voer het toolchain bestand uit, indien nog niet aangeroepen
	include(${CMAKE_CURRENT_LIST_DIR}/ARMCortexMToolchain.cmake)

	set(CMAKE_C_FLAGS CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS CACHE STRING "" FORCE)

	set(CMAKE_SHARED_LINKER_FLAGS_INIT CACHE STRING "" FORCE)
	set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS CACHE STRING "" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS CACHE STRING "" FORCE)
	set(CMAKE_EXEC_LINKER_FLAGS CACHE STRING "" FORCE)

	set (CMAKE_ASM_BASISVLAGGEN "-c -mthumb -mno-unaligned-access -I. -x assembler-with-cpp ${PROJECT_ASM_FLAGS}")

	set (CMAKE_ASM_CM0VLAGGEN "-mcpu=cortex-m0 ${CMAKE_ASM_BASISVLAGGEN}")
	set (CMAKE_ASM_CM3VLAGGEN "-mcpu=cortex-m3 ${CMAKE_ASM_BASISVLAGGEN}")
	set (CMAKE_ASM_CM4VLAGGEN "-mcpu=cortex-m4 ${CMAKE_ASM_BASISVLAGGEN}")
	# -Wa,-adhlns=${OBJECT}.lst
	set (CMAKE_C_BASISVLAGGEN "--std=c++14 -Wall -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -Wno-psabi -mthumb -mno-unaligned-access -mapcs-frame -pipe -funsigned-char -ffunction-sections -fdata-sections")
	set (CMAKE_C_CM0VLAGGEN "-msoft-float -mcpu=cortex-m0 ${CMAKE_C_BASISVLAGGEN} ")
	set (CMAKE_C_CM3VLAGGEN "-msoft-float -mcpu=cortex-m3 -mfix-cortex-m3-ldrd ${CMAKE_C_BASISVLAGGEN} ")
	set (CMAKE_C_CM4FVLAGGEN "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -mfix-cortex-m3-ldrd ${CMAKE_C_BASISVLAGGEN} ")

	set (CMAKE_CXX_BASISVLAGGEN "--std=c++14 -fno-rtti -fno-exceptions --std=gnu++0x -Wctor-dtor-privacy -Wnon-virtual-dtor -finline-limit=20")
	set (CMAKE_CXX_CM0VLAGGEN "${CMAKE_C_CM0VLAGGEN} ${CMAKE_CXX_BASISVLAGGEN}")
	set (CMAKE_CXX_CM3VLAGGEN "${CMAKE_C_CM3VLAGGEN} ${CMAKE_CXX_BASISVLAGGEN}")
	set (CMAKE_CXX_CM4FVLAGGEN "${CMAKE_C_CM4FVLAGGEN} ${CMAKE_CXX_BASISVLAGGEN}")

	# nano.specs geeft aan linker aan dat nano newlib moet worden gebruikt.
	set(StandaardLinkVlaggen "--specs=nano.specs -mthumb -Wl,--gc-sections -Wl,-Map,${PROJEKTNAAM}.map -nostartfiles -Wl,--no-warn-rwx-segments")
	
	if(NOT ONTVLOOI)
	#  set(StandaardLinkVlaggen "--specs=nano.specs -mthumb -Wl,--gc-sections -Wl,-Map,${PROJEKTNAAM}.map -nostartfiles")
	#else(ONTVLOOI)
  	  set(StandaardLinkVlaggen "${StandaardLinkVlaggen} -Wl,--strip-debug")
	endif(NOT ONTVLOOI)
	
	if((WIN32) OR (GebruikCLion) OR ($ENV{CLION_IDE}))
		# Bouw onder windows met CLion
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Executable/${CMAKE_BUILD_TYPE} )
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Executable/${CMAKE_BUILD_TYPE} )
		message(STATUS "Build output dir is : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	endif((WIN32) OR (GebruikCLion) OR ($ENV{CLION_IDE}))

	function(MingWBuildAktie UitvoerbaarBestand ExtraBieb)
		if(WIN32)
			# converteer .elf.exe naar .elf en voeg bouwinfo toe aan de output
			add_custom_command(TARGET ${UitvoerbaarBestand} PRE_BUILD
			                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ExtraBieb} ${TOOLCHAINDIR}/arm-none-eabi/lib
			                   COMMENT "Mingw pre-aktie: ${ExtraBieb} gekopieerd naar ${TOOLCHAINDIR}/arm-none-eabi/lib.\n")
		endif(WIN32)
	endfunction()

	# info,strip macros en kopieer naar van elf.exe naar .elf
	# voor info na het bouwen
	function(PostBuildAktie UitvoerbaarBestand Commentaar)

		# plak eventueel .elf achter de bestandsnaam
		string(FIND ${UitvoerbaarBestand} ".elf" positie)
		if(positie EQUAL -1)
			set(UitvoerbaarBestand "${UitvoerbaarBestand}.elf")
		endif(positie EQUAL -1)

		# vind de naam voor het map bestand
		string(REPLACE ".elf" ".map" MapBestand ${UitvoerbaarBestand})
		# voor debug : message(STATUS "map = ${MapBestand}")

		if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
			set(outputdir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		else(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
			set(outputdir ${CMAKE_CURRENT_BINARY_DIR})
		endif(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

		set(goedenaam ${outputdir}/${UitvoerbaarBestand})
		#message(STATUS "info = ${COMP_INFO_TOOL}")

		if(ONTVLOOI)

			if(NOT WIN32)
				# voeg bouw info to aan de output
				add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
				                  COMMAND ${COMP_INFO_TOOL} -A -x ${goedenaam}
				                  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MapBestand} ${outputdir}/${MapBestand}
				                  DEPENDS ${goedenaam}
				                  COMMENT "Mac OS X post-aktie: ${MapBestand} naar outputdir gekopieerd.\n${Commentaar}")
			else(NOT WIN32)

				set(oudenaam ${outputdir}/${UitvoerbaarBestand}.exe)

				# converteer .elf.exe naar .elf en voeg bouwinfo toe aan de output
				add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
				                   COMMAND ${CMAKE_COMMAND} -E rename ${oudenaam} ${goedenaam}
				                   COMMAND ${COMP_INFO_TOOL} -A -d ${goedenaam}
						           # voor debug van sekties COMMAND ${COMP_READELF} -l ${goedenaam}
				                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MapBestand} ${outputdir}/${MapBestand}
				                   DEPENDS ${oudenaam}
				                   COMMENT "Windows post-aktie: ${MapBestand} naar outputdir gekopieerd.\n${Commentaar}")
			endif(NOT WIN32)
		else(ONTVLOOI)
			# release build. Maak ook een kopie met het versienummer er in
			if(VERSIEHOOFDNUMMER)
				set(VersieString "_${VERSIEHOOFDNUMMER}")
				if(VERSIEONDERNUMMER)
					set(VersieString "${VersieString}_${VERSIEONDERNUMMER}")
					if(VERSIESUBONDERLETTER)
						set(VersieString "${VersieString}_${VERSIESUBONDERLETTER}")
					endif(VERSIESUBONDERLETTER)
				endif(VERSIEONDERNUMMER)
			endif(VERSIEHOOFDNUMMER)

			if(NOT VersieString)
				set(releasenaam ${goedenaam})
			else(NOT VersieString)
				# Voor debug : message("Versie=${VersieString}")
				string(REPLACE ".elf" "${VersieString}.elf" releasenaam ${goedenaam})
			endif(NOT VersieString)

			get_filename_component(goedebestandnaam ${goedenaam} NAME)
			get_filename_component(releasebestandnaam ${releasenaam} NAME)

			string(REPLACE ".elf" ".hex" releasehexnaam ${releasenaam})
			string(REPLACE ".elf" ".bin" releasebinnaam ${releasenaam})
			get_filename_component(releasehexbestandnaam ${releasehexnaam} NAME)
			get_filename_component(releasebinbestandnaam ${releasebinnaam} NAME)

			#   message(STATUS "relnaam is ${releasenaam}")

			if(NOT WIN32)
				# voeg bouw info to aan de output
				add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
				                   COMMAND ${CMAKE_STRIP} -s ${goedenaam}
				                   COMMAND ${COMP_INFO_TOOL} -A -d ${goedenaam}
				                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MapBestand} ${outputdir}/${MapBestand}
				                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${goedenaam} ${releasenaam}
				                   COMMAND ${CMAKE_OBJCOPY} --output-format=ihex ${goedenaam} ${releasehexnaam}
				                   COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${goedenaam} ${releasebinnaam}
				                   DEPENDS ${goedenaam}
				                   COMMENT "Unix post build aktie:\n -- ${releasebestandnaam} naar ${releasehexbestandnaam} geconverteerd.\n -- ${releasebestandnaam} naar ${releasebinbestandnaam} geconverteerd.\n${Commentaar}")
			else(NOT WIN32)

				set(oudenaam ${outputdir}/${UitvoerbaarBestand}.exe)

				# converteer .elf.exe naar .elf en voeg bouwinfo toe aan de output
				add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
				                   COMMAND ${CMAKE_STRIP} -s ${oudenaam}
				                   COMMAND ${COMP_INFO_TOOL} -A -d ${oudenaam}
				                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MapBestand} ${outputdir}/${MapBestand}
				                   COMMAND ${CMAKE_COMMAND} -E rename ${oudenaam} ${goedenaam}
				                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${goedenaam} ${releasenaam}
				                   COMMAND ${CMAKE_OBJCOPY} --output-format=ihex ${goedenaam} ${releasehexnaam}
				                   COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${goedenaam} ${releasebinnaam}
				                   DEPENDS ${oudenaam}
				                   COMMENT "Windows post build aktie:\n -- ${UitvoerbaarBestand}.exe naar ${UitvoerbaarBestand} gekopieerd.\n -- ${goedebestandnaam} naar ${releasebestandnaam} gekopieerd.\n -- ${releasebestandnaam} naar ${releasehexbestandnaam} geconverteerd.\n${Commentaar}")
			endif(NOT WIN32)

		endif(ONTVLOOI)

		# niet meer   MaakAllesSchoon()
	endfunction()

	function(MaakLijst Bronnen)

		# Windoze heeft quotes nodig om een bestand met spaties (MS OneDrive) 
		# te kunnen compileren.
		# maak een lst bestand aan voor elke gecomplieerd bestand
		foreach(bestand ${Bronnen})
			get_filename_component(naam ${bestand} NAME)
			set_source_files_properties (${bestand}
			                             PROPERTIES
			                             COMPILE_FLAGS "-Wa,-ahl=\"${CMAKE_CURRENT_BINARY_DIR}/${naam}.lst\"")
		endforeach(bestand in ${Bronnen})
	endfunction(MaakLijst Bronnen)

	function(MaakLib Naam Bronnen)

		MaakLijst("${Bronnen}")
		# maak de library
		add_library(${Naam} STATIC "${Bronnen}")

	endfunction(MaakLib Naam Bronnen)

	function(MaakSeggerOzoneConfig UitvoerbaarBestand SeggerJLinkControllerID
	         SeggerJLinkCoreSVDNaam SeggerJLinkMicrocontrollerSVDNaam SeggerJLinkProtokol SeggerJLinkID SeggerJLinkSnelheid)

		if(NOT ONTVLOOI)
			message(STATUS "Geen debug mode derhalve geen JLink Ozone config bestand gemaakt.")
		else(NOT ONTVLOOI)
			#CMAKE_PARSE_ARGUMENTS(MaakSeggerOzoneConfig SeggerJLinkControllerID SeggerJLinkCoreSVDNaam SeggerJLinkMicrocontrollerSVDNaam UitvoerbaarBestand )
			if(NOT SeggerJLinkControllerID)
				message(FATAL ERROR "SeggerJLinkControllerID (bijv STM32L476VG) MOET worden gezet om deze funktie uit kunnen voeren")
			endif(NOT SeggerJLinkControllerID)

			#if(NOT SeggerJLinkID)
			# zet op nul om alle interfaces aan te kunnen spreken
			#       set(SeggerJLinkID 0)
			#endif(NOT SeggerJLinkID)

			#if(NOT SeggerJLinkSnelheid)
			#       set(SeggerJLinkSnelheid auto)
			#endif(NOT SeggerJLinkSnelheid)

			#if(NOT SeggerJLinkCoreSVDNaam)
			#       message(FATAL ERROR "SeggerJLinkControllerID (bijv Cortex-M4.svd) MOET worden gezet om deze funktie uit kunnen voeren")
			#endif(NOT SeggerJLinkCoreSVDNaam)


			# Configureer een header bestand met vanuit cmake bekende parameters
			# zoals versienummers en copyright velden
			set(SeggerOzoneConfigDir ${PROJECT_BINARY_DIR}/gegenereerd)
			set(SeggerOzoneConfigBestand "${SeggerOzoneConfigDir}/ozoneConfig.jdebug")
			configure_file (${STM32BasisDir}/Segger/JLink/ozoneprotobestand.jdebug.in ${SeggerOzoneConfigBestand})
			message(STATUS "${SeggerOzoneConfigBestand} werd gegenereerd.")
		endif(NOT ONTVLOOI)
	endfunction()

	function(MaakSeggerSimpelOzoneConfigCortexM0 UitvoerbaarBestand SeggerJLinkControllerID
	         SeggerJLinkMicrocontrollerSVDNaam)

		set(SeggerJLinkCoreSVDNaam "Cortex-M0.svd")
		set(SeggerJLinkProtokol SWD)
		set(SeggerJLinkID 0)
		set(SeggerJLinkSnelheid auto)

		MaakSeggerOzoneConfig(${UitvoerbaarBestand} ${SeggerJLinkControllerID} ${SeggerJLinkCoreSVDNaam}
		                      ${SeggerJLinkMicrocontrollerSVDNaam} ${SeggerJLinkProtokol} ${SeggerJLinkID} ${SeggerJLinkSnelheid})
	endfunction()

	function(MaakSeggerSimpelOzoneConfigCortexM3 UitvoerbaarBestand SeggerJLinkControllerID
	         SeggerJLinkMicrocontrollerSVDNaam)

		set(SeggerJLinkCoreSVDNaam "Cortex-M3.svd")
		set(SeggerJLinkProtokol SWD)
		set(SeggerJLinkID 0)
		set(SeggerJLinkSnelheid auto)

		MaakSeggerOzoneConfig(${UitvoerbaarBestand} ${SeggerJLinkControllerID} ${SeggerJLinkCoreSVDNaam}
		                      ${SeggerJLinkMicrocontrollerSVDNaam} ${SeggerJLinkProtokol} ${SeggerJLinkID} ${SeggerJLinkSnelheid})
	endfunction()

	function(MaakSeggerSimpelOzoneConfigCortexM4F UitvoerbaarBestand SeggerJLinkControllerID
	         SeggerJLinkMicrocontrollerSVDNaam)

		set(SeggerJLinkCoreSVDNaam "Cortex-M4F.svd")
		set(SeggerJLinkProtokol SWD)
		set(SeggerJLinkID 0)
		set(SeggerJLinkSnelheid auto)

		MaakSeggerOzoneConfig(${UitvoerbaarBestand} ${SeggerJLinkControllerID} ${SeggerJLinkCoreSVDNaam}
		                      ${SeggerJLinkMicrocontrollerSVDNaam} ${SeggerJLinkProtokol} ${SeggerJLinkID} ${SeggerJLinkSnelheid})
	endfunction()

	macro(MaakTijdStempel UitvoerbaarBestand TijdBestand)
		# build time in UTC ISO 8601
		string(TIMESTAMP StempelTekst "%Y-%m-%d %H:%M")
		message(STATUS "Link stempeltijd = ${StempelTekst}")
		file (WRITE ${CMAKE_BINARY_DIR}/timestamp.cmake "string(TIMESTAMP DatumStempelTekst \"%d-%m-%Y\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "string(TIMESTAMP TijdStempelTekst \"%H:%M:%S\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "set(pad \"${PROJECT_BINARY_DIR}/gegenereerd/${TijdBestand}\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "file(WRITE \${pad} \"/* Automatisch gegenereerd bestand. */\\n\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "file(APPEND \${pad} \"#ifndef TijdStempel_H\\n\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "file(APPEND \${pad} \"#define TijdStempel_H\\n\\n\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "file(APPEND \${pad} \"#define TijdStempel \\\"\${TijdStempelTekst} \${DatumStempelTekst}\\\"\\n\\n\")\n")
		file (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "file(APPEND \${pad} \"#endif // TijdStempel_H\\n\")\n")
		add_custom_target(
				TijdStempel
				COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/timestamp.cmake
				ADD_DEPENDENCIES ${CMAKE_BINARY_DIR}/timestamp.cmake)
		
		add_dependencies(${UitvoerbaarBestand} TijdStempel)
	endmacro()
	