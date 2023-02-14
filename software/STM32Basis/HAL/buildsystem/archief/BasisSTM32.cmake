## Copyright 2011-2014 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: BasisSTM32.cmake 4390 2022-02-24 14:44:20Z ewout $

message(STATUS "HAN ESE STM32 software ontwikkeling.")

set(ARMCORTEXM ON)

if(OverrideGCC_G++)
  set(GCCNaam g++)
else(OverrideGCC_G++)
  set(GCCNaam gcc)
endif(OverrideGCC_G++)

#OPTION(ONTVLOOI "Bouw het bestand met debug informatie" OFF)
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
else(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(ONTVLOOI ON)
    message(STATUS "Software wordt gebouwd met debugging opties")  
    add_definitions(-Os -fno-default-inline)
  else()
    set(ONTVLOOI OFF)
    message(STATUS "Software wordt gebouwd met optimalisatie opties")  
    add_definitions(-Os -DNDEBUG)
  endif()

endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

ENABLE_LANGUAGE (ASM-ATT OPTIONAL) 

set (CMAKE_FIND_ROOT_PATH_MODE NEVER)

if(UNIX)
  message(STATUS "Unix gedetecteerd.")

  find_path(GNUARMBasisDir bin/arm-none-eabi-g++ PATHS /usr/local /opt/local /usr/ HINTS /usr/local/arm-none-eabi NO_DEFAULT_PATH)

  if(NOT GNUARMBasisDir)
    message(FATAL_ERROR "GNU ARM EABI pad werd niet gevonden.")
  else(NOT GNUARMBasisDir)

    set(TOOLCHAINDIR ${GNUARMBasisDir})

    SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAINDIR})
    SET(CMAKE_C_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-${GCCNaam})
    SET(CMAKE_CXX_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-g++)
    SET(CMAKE_ASM-ATT_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-gcc)
    set(CMAKE_RANLIB ${TOOLCHAINDIR}/bin/arm-none-eabi-ranlib)
    set(COMP_INFO_TOOL  ${TOOLCHAINDIR}/bin/arm-none-eabi-size)

    set(COMP_INFO_TOOL ${TOOLCHAINDIR}/bin/arm-none-eabi-size)
    set(CMAKE_STRIP ${TOOLCHAINDIR}/bin/arm-none-eabi-strip)
    set(CMAKE_OBJCOPY ${TOOLCHAINDIR}/bin/arm-none-eabi-objcopy)

    if (APPLE)
      message(STATUS "Apple platform gedetecteerd.")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
      message(STATUS "FreeBSD platform gedetecteerd.")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      message(STATUS "SUN Solaris/Linux platform gedetecteerd.")
      # om -r dynamic te omzeilen aan het einde van de linkopdracht
      set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
      set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
    else()
      message(FATAL_ERROR "Geen ondersteund Unix platform gevonden")
    endif(APPLE)
    message(STATUS "GNU ARM compiler gevonden op : ${TOOLCHAINDIR}")
  endif(NOT GNUARMBasisDir)
else(UNIX)

  if(WIN32)

    message(STATUS "Microsoft Windows omgeving gedetecteerd.")

    set(CMAKE_SYSTEM_NAME "Generic")
    set(CMAKE_FIND_ROOT_PATH_MODE NEVER)

    if(POLICY CMP0053)
	    cmake_policy(SET CMP0053 OLD)
    endif(POLICY CMP0053)

    set(GNUARMWinBasisdir "$ENV{PROGRAMFILES}/GNU\ Tools\ ARM\ Embedded")
    set(GNUARMWINVersies  "4.8\ 2014q3"  "5.2\ 2015q4" "5.3\ 2016q1" "5.4\ 2016q2")
    
    foreach(GNUARMWINVersie ${GNUARMWINVersies})
      
      set(GNUARMWINDir ${GNUARMWinBasisdir}/${GNUARMWINVersie})

      if(EXISTS ${GNUARMWINDir})
	set(TOOLCHAINDIR  ${GNUARMWINDir})
	message(STATUS "GNU ARM Toolchain gevonden op ${GNUARMWINDir}")
      else()
	message(STATUS "Geen GNU ARM Toolchain gevonden op ${GNUARMWINDir}")
      endif(EXISTS ${GNUARMWINDir})

    endforeach(GNUARMWINVersie)
    
    if(NOT TOOLCHAINDIR)
      message(FATAL_ERROR "Geen GNU ARM Toolchain gevonden in ${GNUARMWinBasisdir}")
    else(NOT TOOLCHAINDIR)
      message(STATUS "GNU ARM Toolchain op ${GNUARMWINDir} wordt gebruikt.")
      set(GNUARMBasisDir ${TOOLCHAINDIR})
    endif(NOT TOOLCHAINDIR)
    
    set(CMAKE_FIND_ROOT_PATH ${TOOLCHAINDIR})

    set(CMAKE_C_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-${GCCNaam}.exe)
    set(CMAKE_CXX_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-g++.exe)
    set(CMAKE_ASM-ATT_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-${GCCNaam}.exe)
    set(CMAKE_RANLIB ${TOOLCHAINDIR}/bin/arm-none-eabi-ranlib.exe)
    set(CMAKE_AR ${TOOLCHAINDIR}/bin/arm-none-eabi-ar.exe)
    set(CMAKE_NM ${TOOLCHAINDIR}/bin/arm-none-eabi-nm.exe)
    set(COMP_INFO_TOOL  ${TOOLCHAINDIR}/bin/arm-none-eabi-size.exe)

    set(COMP_INFO_TOOL  ${GNUARMBasisDir}/bin/arm-none-eabi-size.exe)
    set(CMAKE_STRIP ${GNUARMBasisDir}/bin/arm-none-eabi-strip.exe)
    set(CMAKE_OBJCOPY ${GNUARMBasisDir}/bin/arm-none-eabi-objcopy.exe)
    
    set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
    set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "" FORCE) 

    #Linking command om  MinGW opzet te omzeilen
    set( CMAKE_C_LINK_EXECUTABLE
      "\"${CMAKE_C_COMPILER}\" <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> -o <TARGET>"
      )
    set( CMAKE_CXX_LINK_EXECUTABLE
      "\"${CMAKE_CXX_COMPILER}\" <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> -o <TARGET>"
      )
    
  else(Win32)
    message(FATAL_ERROR "Onbekend platform aangetroffen (Geen Unix/Geen MS Windows)")
  endif(WIN32)

endif(UNIX)

set(CMAKE_C_FLAGS CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS CACHE STRING "" FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_INIT CACHE STRING "" FORCE)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS CACHE STRING "" FORCE)
set(CMAKE_EXEC_LINKER_FLAGS CACHE STRING "" FORCE)

SET (CMAKE_ASM_BASISVLAGGEN "-c -mthumb -mthumb-interwork -Wa,-ggdb -I. -x assembler-with-cpp ${PROJECT_ASM_FLAGS}")

SET (CMAKE_ASM_CM0VLAGGEN "-mcpu=cortex-m0 ${CMAKE_ASM_BASISVLAGGEN}")
SET (CMAKE_ASM_CM3VLAGGEN "-mcpu=cortex-m3 ${CMAKE_ASM_BASISVLAGGEN}")
SET (CMAKE_ASM_CM4VLAGGEN "-mcpu=cortex-m4 ${CMAKE_ASM_BASISVLAGGEN}")
# -Wa,-adhlns=${OBJECT}.lst 
set (CMAKE_C_BASISVLAGGEN "-Wall -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -Werror -Wno-psabi -mthumb -mthumb-interwork -mapcs-frame -pipe -funsigned-char -ffunction-sections -fdata-sections ")
set (CMAKE_C_CM0VLAGGEN "-msoft-float -mcpu=cortex-m0 ${CMAKE_C_BASISVLAGGEN} ")
set (CMAKE_C_CM3VLAGGEN "-msoft-float -mcpu=cortex-m3 -mfix-cortex-m3-ldrd ${CMAKE_C_BASISVLAGGEN} ")
set (CMAKE_C_CM4FVLAGGEN "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -mfix-cortex-m3-ldrd ${CMAKE_C_BASISVLAGGEN} ")

set (CMAKE_CXX_BASISVLAGGEN "-fno-rtti -fno-exceptions --std=gnu++0x -Wctor-dtor-privacy -Wnon-virtual-dtor -finline-limit=20")
set (CMAKE_CXX_CM0VLAGGEN "${CMAKE_C_CM0VLAGGEN} ${CMAKE_CXX_BASISVLAGGEN}")
set (CMAKE_CXX_CM3VLAGGEN "${CMAKE_C_CM3VLAGGEN} ${CMAKE_CXX_BASISVLAGGEN}")
set (CMAKE_CXX_CM4FVLAGGEN "${CMAKE_C_CM4FVLAGGEN} ${CMAKE_CXX_BASISVLAGGEN}")

# nano.specs geeft aan linker aan dat nano newlib moet worden gebruikt.		
set(StandaardLinkVlaggen "--specs=nano.specs -mthumb -Wl,--gc-sections -Wl,-Map,${PROJECTNAAM}.map -nostartfiles")

if((WIN32) OR (GebruikCLion) OR $ENV{CLION_IDE})
  # Bouw onder windows met CLion
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Executable/${CMAKE_BUILD_TYPE} )
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Executable/${CMAKE_BUILD_TYPE} )
  message(STATUS "Build output dir is : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif((WIN32) OR (GebruikCLion) OR $ENV{CLION_IDE})

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
  #message(STATUS "map = ${MapBestand}")
  
  if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(outputdir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  else(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(outputdir ${CMAKE_CURRENT_BINARY_DIR})
  endif(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  
  set(goedenaam ${outputdir}/${UitvoerbaarBestand})
  
  if(ONTVLOOI)
    
    if(NOT WIN32)
      # voeg bouw info to aan de output
      add_custom_target(info ALL
        COMMAND ${COMP_INFO_TOOL} -A -d ${goedenaam}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MapBestand} ${outputdir}/${MapBestand}
        DEPENDS ${goedenaam}
        COMMENT ${Commentaar})
    else(NOT WIN32)
      
      set(oudenaam ${outputdir}/${UitvoerbaarBestand}.exe)
      
      # converteer .elf.exe naar .elf en voeg bouwinfo toe aan de output
      add_custom_command(TARGET ${UitvoerbaarBestand} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename ${oudenaam} ${goedenaam}
		COMMAND ${COMP_INFO_TOOL} -A -d ${goedenaam}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MapBestand} ${outputdir}/${MapBestand}
        DEPENDS ${oudenaam}
        COMMENT "Windows post-aktie: ${UitvoerbaarBestand}.exe naar ${UitvoerbaarBestand} gekopieerd.\n${Commentaar}")
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
  
  # maak een lst bestand aan voor elke gecomplieerd bestand
  foreach(bestand ${Bronnen})
    get_filename_component(naam ${bestand} NAME)
    set_source_files_properties (${bestand}
      PROPERTIES
      COMPILE_FLAGS "-Wa,-ahl=${CMAKE_CURRENT_BINARY_DIR}/${naam}.lst")
  endforeach(bestand in ${Bronnen})

endfunction(MaakLijst Bronnen)

function(MaakLib Naam Bronnen)

  MaakLijst("${Bronnen}")
  # maak de library
  add_library(${Naam} STATIC "${Bronnen}")
  
endfunction(MaakLib Naam Bronnen)
