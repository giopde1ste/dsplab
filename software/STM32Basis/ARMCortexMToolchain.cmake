## Copyright 2011-2022 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: ARMCortexMToolchain.cmake 4634 2022-11-10 16:11:28Z ewout $

if(NOT ARMCortexMToolchainGevonden)
	message(STATUS "ARM Cortex M architectuur toolchain initialisatie.")

	cmake_policy(SET CMP0077 NEW)
	#set(CMAKE_SYSTEM_NAME "Generic")
	set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
	option(CMAKE_C_COMPILER_WORKS "De C Compiler werkt." ON)
	option(CMAKE_CXX_COMPILER_WORKS "De C++ Compiler werkt." ON)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

	option(ARMCORTEXM "Bouw voor ARM Cortex-M architectuur" ON)

	if(OverrideGCC_G++)
		set(GCCNaam g++ CACHE STRING "C++ compiler")
	else(OverrideGCC_G++)
		set(GCCNaam gcc CACHE STRING "C compiler")
	endif(OverrideGCC_G++)

	#ENABLE_LANGUAGE (ASM-ATT OPTIONAL)

	if(UNIX)
		message(STATUS "Unix gedetecteerd.")

		set(GNUARMBasisZoekDirs "/usr" "/usr/local" "/usr/local/arm-none-eabi")
		set(GNUARMVersies  "bin")

		foreach(GNUARMBasisZoekDir ${GNUARMBasisZoekDirs})
			foreach(GNUARMVersie ${GNUARMVersies})
				set(GNUARMDir ${GNUARMBasisZoekDir}/${GNUARMVersie}/arm-none-eabi-gcc)

				if(EXISTS ${GNUARMDir})
					set(GNUARMBasisDir  ${GNUARMBasisZoekDir})
					message(STATUS "GNU ARM Toolchain gevonden op ${GNUARMDir}")
				else(EXISTS ${GNUARMDir})
					#message(STATUS "Geen GNU ARM Toolchain gevonden op ${GNUARMDir}")
				endif(EXISTS ${GNUARMDir})
			endforeach(GNUARMVersie)
		endforeach(GNUARMBasisZoekDir ${GNUARMBasisZoekDirs})


		##########################################################################
		# executables in use
		##########################################################################
		#find_program(ARMEABI_CC arm-eabi-gcc)
		#find_program(AVR_CXX avr-g++ /opt/local)
		#find_program(AVR_OBJCOPY avr-objcopy)
		#find_program(AVR_SIZE_TOOL avr-size)
		#find_program(AVR_OBJDUMP avr-objdump)

		if(GNUARMBasisDir)
			set(TOOLCHAINDIR ${GNUARMBasisDir})
		else()
			set(GNUARMVersies "gcc-arm-none-eabi-" "arm-none-eabi-")
			set(GNUARMVersieNummers
			    "4.8-2014q3"
			    "5.2-2015q4" "5.3-2016q1" "5.4-2016q3"
			    "6-2017-q2-update"
			    "7-2017-q4-major" "7-2018-q2-update"
			    "8-2018-q4-major" "8-2019-q3-update"
			    "9-2019-q4-major" "9-2020-q2-update"
			    "10-2020-q4-major" "10-2021-21.10"
			    "11.2\ 2022.02" "11.3.Rel1"
			    "12.2.MPACBTI-Bet1" "12.2.Rel1"
			    "ARM")

			foreach(GNUARMBasisZoekDir ${GNUARMBasisZoekDirs})
				foreach(GNUARMVersie ${GNUARMVersies})
					foreach(GNUARMVersieNummer ${GNUARMVersieNummers})

						set(GNUARMDir ${GNUARMBasisZoekDir}/${GNUARMVersie}${GNUARMVersieNummer})

						if(EXISTS ${GNUARMDir})
							set(GNUARMBasisDir  ${GNUARMDir})
							message(STATUS "GNU ARM Toolchain gevonden op ${GNUARMDir}")
						else()
							#message(STATUS "Geen GNU ARM Toolchain gevonden op ${GNUARMDir}")
						endif(EXISTS ${GNUARMDir})
					endforeach(GNUARMVersieNummer)
				endforeach(GNUARMVersie)
			endforeach(GNUARMBasisZoekDir ${GNUARMBasisZoekDirs})

			set(TOOLCHAINDIR ${GNUARMBasisDir})
		endif()
		if(NOT GNUARMBasisDir)
			message(FATAL_ERROR "Geen GNU ARM Toolchain gevonden in ${GNUARMBasisdir}")
		else(NOT TOOLCHAINDIR)
			message(STATUS "====> GNU ARM Toolchain op ${TOOLCHAINDIR} wordt gebruikt.")
			set(CMAKE_FIND_ROOT_PATH ${TOOLCHAINDIR})
			set(CMAKE_C_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-${GCCNaam})
			set(CMAKE_CXX_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-g++)
			set(CMAKE_ASM-ATT_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-gcc)
			set(CMAKE_RANLIB ${TOOLCHAINDIR}/bin/arm-none-eabi-ranlib)
			set(COMP_INFO_TOOL  ${TOOLCHAINDIR}/bin/arm-none-eabi-size)
			set(COMP_READELF ${TOOLCHAINDIR}/bin/arm-none-eabi-readelf)
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
				set(ARMCortexMToolchainGevonden OFF)
				message(FATAL_ERROR "Geen ondersteund Unix platform gevonden")
			endif(APPLE)
			message(STATUS "GNU ARM compiler gevonden op : ${TOOLCHAINDIR}")
			set(ARMCortexMToolchainGevonden ON)
		endif(NOT GNUARMBasisDir)

	else(UNIX)
		if(WIN32)
			message(STATUS "Microsoft Windows omgeving gedetecteerd.")

			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

			set(CMAKE_SYSTEM_NAME "Generic")
			set(CMAKE_FIND_ROOT_PATH_MODE NEVER)

			if(POLICY CMP0053)
				cmake_policy(SET CMP0053 NEW)
			endif(POLICY CMP0053)

			set(hv "4" "5" "6" "7" "8" "9")
			set(mv "0" "1" "2" "3" "4" "5" "6" "7" "8" "9")
			set(ov ${mv})

			foreach(hvvar ${hv})
				foreach(mvvar ${mv})
					foreach(ovvar ${ov})
						set(MinGWVersieTest "${hvvar}.${mvvar}.${ovvar}")
						set(MINGWGCCLibdirTest "D:/mingw-w64/mingw32/lib/gcc/i686-w64-mingw32/${MinGWVersieTest}")
						#message(STATUS "[Debug] MingGW test op : ${MINGWGCCLibdirTest}")
						if(EXISTS ${MINGWGCCLibdirTest})
							message(STATUS "MingGW gevonden op : ${MINGWGCCLibdirTest}")
							set(MINGWGCCLibdir ${MINGWGCCLibdirTest})
						endif(EXISTS ${MINGWGCCLibdirTest})
					endforeach()
				endforeach()
			endforeach()

			if(NOT EXISTS ${MINGWGCCLibdir})
				message(FATAL_ERROR "De MinGW GCC lib dir werd NIET gevonden!")
			else(NOT EXISTS ${MINGWGCCLibdir})
				# Dec 2017 : bug gcc 5.4 in mingw omgeving. Dit moet eenmalig handmatig worden gedaan als admin.
				set(EHinLibdir ${TOOLCHAINDIR}/arm-none-eabi/lib/)
				set(EHbestand ${EHinLibdir}/libgcc_eh.a)
				#if(NOT EXISTS ${EHbestand})
				#	message(FATAL_ERROR "De MinGW libgcc_eh.a werd NIET in ${EHinLibdir} aangetroffen.\n kopieer als Admin ${MINGWGCCLibdir}/libgcc_eh.a naar ${EHinLibdir}")
				#else(NOT EXISTS ${EHbestand})
				#   message(STATUS "(MinGW basis : libgcc_eh.a aangetroffen op ARM GCC lib pad.")
				#endif(NOT EXISTS ${EHbestand})

			endif(NOT EXISTS ${MINGWGCCLibdir})

			set(MYENV86 "PROGRAMFILES(x86)")
			set(MYENV "C:\\Program\ Files")

			set(WindowsProgDir "C:/Program\ Files\ (x86)")

			set(GNUARMWinBasisDirectories "${WindowsProgDir}/GNU\ Tools\ ARM\ Embedded"
			    "${WindowsProgDir}/GNU\ Arm\ Embedded\ Toolchain"
			    "${WindowsProgDir}/ARM\ GNU\ Toolchain\ arm-none-eabi")

			foreach(GNUARMWinBasisdir ${GNUARMWinBasisDirectories})
				# let op stel op in opvolgende volgorde zodat de laatste blijft hangen
				set(GNUARMWINVersies
				    "4.8\ 2014q3"
				    "5.2\ 2015q4"
				    "5.3\ 2016q1" "5.4\ 2016q3"
				    "6\ 2017-q1-update" "6\ 2017-q2-update" "7\ 2017-q4-major"
				    "7\ 2018-q2-update"
				    # 24.06.2019 Deze geeft fout bij ihex conversie "8\ 2018-q4-major")
				    "8\ 2019-q3-update" "9\ 2019-q4-major"
				    "9\ 2020-q2-update" "10\ 2020-q4-major"
				    "10\ 2021.10"
				    "11.2\ 2022.02"
				    "11.3\ rel1"
				    "12.2\ mpacbti-bet1"
				    "12.2\ rel1"
					"12.2\ mpacbti-rel1")

				foreach(GNUARMWINVersie in ${GNUARMWINVersies})
					set(GNUARMWINDir ${GNUARMWinBasisdir}/${GNUARMWINVersie})

					if(EXISTS ${GNUARMWINDir})
						set(TOOLCHAINDIR  ${GNUARMWINDir})
						message(STATUS "GNU ARM Toolchain gevonden op ${GNUARMWINDir}")
					else()
						#message(STATUS "Geen GNU ARM Toolchain gevonden op ${GNUARMWINDir}")
					endif(EXISTS ${GNUARMWINDir})

				endforeach(GNUARMWINVersie)
			endforeach(GNUARMWinBasisdir)

			if(NOT TOOLCHAINDIR)
				message(FATAL_ERROR "Geen GNU ARM Toolchain gevonden in ${GNUARMWinBasisdir}")
			else(NOT TOOLCHAINDIR)
				message(STATUS "====> GNU ARM Toolchain op ${TOOLCHAINDIR} wordt gebruikt.")
				set(GNUARMBasisDir ${TOOLCHAINDIR})
				message(STATUS "GNU ARM compiler gevonden op : ${TOOLCHAINDIR}")
				set(ARMCortexMToolchainGevonden ON)
			endif(NOT TOOLCHAINDIR)

			set(CMAKE_FIND_ROOT_PATH ${TOOLCHAINDIR})

			set(CMAKE_C_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-${GCCNaam}.exe)
			set(CMAKE_CXX_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-g++.exe)
			set(CMAKE_ASM-ATT_COMPILER ${TOOLCHAINDIR}/bin/arm-none-eabi-${GCCNaam}.exe)
			set(CMAKE_RANLIB ${TOOLCHAINDIR}/bin/arm-none-eabi-ranlib.exe)
			set(CMAKE_AR ${TOOLCHAINDIR}/bin/arm-none-eabi-ar.exe)
			set(CMAKE_NM ${TOOLCHAINDIR}/bin/arm-none-eabi-nm.exe)
			set(COMP_INFO_TOOL  ${TOOLCHAINDIR}/bin/arm-none-eabi-size.exe)
			set(COMP_READELF ${TOOLCHAINDIR}/bin/arm-none-eabi-readelf.exe)
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

endif(NOT ARMCortexMToolchainGevonden)


if(NOT ARMCortexMToolchainGevonden)
	message(FATAL_ERROR "De ARM Cortex-M toolchain is niet gevonden. Installeer de compiler suite vanaf : https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm")
endif(NOT ARMCortexMToolchainGevonden)
