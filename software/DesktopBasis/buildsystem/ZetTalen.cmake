## Kiwanda Embedded Systemen
## Copyright 2001-2021 Kiwanda Embedded Systemen
## $Id: ZetTalen.cmake 4389 2022-02-24 14:42:24Z ewout $

#if(NOT TAALDIR)
#	message(ERROR "TAALDIR is niet gezet, de talen kunnen hier niet worden gezet en gekopieerd")
#else(NOT TAALDIR)
macro(InstalleerTalen TaalNaam AppNaam TAALDIR)

	set(DESKTOPTAALDIR  ${EXTERNALDIR}/desktop/talen)
	set(Taalkodes nl de fr it es pt sv_SE ru ja_JP zh_CN zh_TW ko_KR he_IL id vi ro)
	list(SORT Taalkodes)
	
	foreach(taal ${Taalkodes})
		# ${TAALDIR}/${taal}_${taal}/*.mo
		file(GLOB catalogus ${TAALDIR}/${taal}/${TaalNaam}.mo )
		# message("[debug] ese taal=${taal} app bestand=${catalogus}")
		file(GLOB wxcatalogus ${TAALDIR}/${taal}/wxstd.mo)
		# message("[debug] wx taal=${taal} wxbestand=${wxcatalogus}")

		if(UNIX)
			if(APPLE)
				#message(STATUS "apple bundle talen")
				# Apple heeft het goed geregeld : taalcatalogi in de bundle
				# De bundle talen zijn onderdeel van de executable, anders dan bij win en unix waar
				# deze ls aparte bestanden worden gekopieerd.
				set_source_files_properties(${catalogus} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${taal}.lproj)
				set_source_files_properties(${wxcatalogus} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${taal}.lproj)
				# Zet deze variabele hieronder, want hij wordt meegenomen in de "add executable"
				list(APPEND TALEN ${catalogus} ${wxcatalogus})
				list(SORT TALEN)
			else(APPLE)

				# Als de zaak al een _ bevat, niet verlengen met hoofdletter extensie.
				string(FIND "${taal}" "_" pos1)
				if(pos1 STREQUAL "-1")
					string(TOUPPER ${taal} uptaal)
					set(inserttaal ${taal}_${uptaal})
				else()
					set(inserttaal ${taal})
				endif()

				# Unix rommelsituatie.
                # De Kiwanda Embedded oplossing :
                # Zet de *.mo bestanden in de <lokaal>/Kiwanda/AppNaam/locale/kode/LC_MESSAGES dir
				# Hiermee wordt ook geklaag bij Debian voorkomen over dubbel kopieeren van KiwandaDesktop.mo
				set(TaalInstallatieDir share/Kiwanda/${AppNaam}/locale/${inserttaal}/LC_MESSAGES)
				install(FILES ${catalogus} ${wxcatalogus}
						DESTINATION ${TaalInstallatieDir}
						PERMISSIONS
						OWNER_WRITE OWNER_READ
						GROUP_READ
						WORLD_READ)

			endif(APPLE)
		else(UNIX)
			if(WIN32)
			
			# Als de zaak al een _ bevat, niet verlengen met hoofdletter extensie.
				string(FIND "${taal}" "_" pos1)
				if(pos1 STREQUAL "-1")
					string(TOUPPER ${taal} uptaal)
					set(inserttaal ${taal}_${uptaal})
				else()
					set(inserttaal ${taal})
				endif()
					
				install(FILES ${catalogus} DESTINATION share/locale/${inserttaal}/LC_MESSAGES)
				install(FILES ${wxcatalogus} DESTINATION share/locale/${inserttaal}/LC_MESSAGES)
			else(WIN32)
				message(FATAL_ERROR "niet ondersteund platform gevonden.")
			endif(WIN32)
		endif(UNIX)
	endforeach(taal)

	#message("[debug] Gevonden talen=${TALEN}")
	#endif(NOT TAALDIR)

endmacro(InstalleerTalen AppNaam TAALDIR)

