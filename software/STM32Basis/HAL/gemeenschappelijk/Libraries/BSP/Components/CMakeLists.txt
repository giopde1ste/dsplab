## Boombaard generated CMakelists add_library file
## File generated on : 2017-07-01 14:32:04.
## (c)2002-2017 Kiwanda Embedded Systemen
## $Id: CMakeLists.txt 4390 2022-02-24 14:44:20Z ewout $

set(LibName ComponentsLib)

set(SrcPath0 ${CMAKE_CURRENT_SOURCE_DIR}/ampire480272)
set(SrcPath1 ${CMAKE_CURRENT_SOURCE_DIR}/ampire640480)
set(SrcPath2 ${CMAKE_CURRENT_SOURCE_DIR}/Common)
set(SrcPath3 ${CMAKE_CURRENT_SOURCE_DIR}/cs43l22)
set(SrcPath4 ${CMAKE_CURRENT_SOURCE_DIR}/exc7200)
set(SrcPath5 ${CMAKE_CURRENT_SOURCE_DIR}/ft6x06)
set(SrcPath6 ${CMAKE_CURRENT_SOURCE_DIR}/ili9325)
set(SrcPath7 ${CMAKE_CURRENT_SOURCE_DIR}/ili9341)
set(SrcPath8 ${CMAKE_CURRENT_SOURCE_DIR}/l3gd20)
set(SrcPath9 ${CMAKE_CURRENT_SOURCE_DIR}/lis302dl)
set(SrcPath10 ${CMAKE_CURRENT_SOURCE_DIR}/lis3dsh)
set(SrcPath11 ${CMAKE_CURRENT_SOURCE_DIR}/ls016b8uy)
set(SrcPath12 ${CMAKE_CURRENT_SOURCE_DIR}/lsm303dlhc)
set(SrcPath13 ${CMAKE_CURRENT_SOURCE_DIR}/mfxstm32l152)
set(SrcPath14 ${CMAKE_CURRENT_SOURCE_DIR}/n25q128a)
set(SrcPath15 ${CMAKE_CURRENT_SOURCE_DIR}/n25q256a)
set(SrcPath16 ${CMAKE_CURRENT_SOURCE_DIR}/n25q512a)
set(SrcPath17 ${CMAKE_CURRENT_SOURCE_DIR}/otm8009a)
set(SrcPath18 ${CMAKE_CURRENT_SOURCE_DIR}/ov2640)
set(SrcPath19 ${CMAKE_CURRENT_SOURCE_DIR}/s25fl512s)
set(SrcPath20 ${CMAKE_CURRENT_SOURCE_DIR}/s5k5cag)
set(SrcPath21 ${CMAKE_CURRENT_SOURCE_DIR}/st7735)
set(SrcPath22 ${CMAKE_CURRENT_SOURCE_DIR}/st7789h2)
set(SrcPath23 ${CMAKE_CURRENT_SOURCE_DIR}/stmpe1600)
set(SrcPath24 ${CMAKE_CURRENT_SOURCE_DIR}/stmpe811)
set(SrcPath25 ${CMAKE_CURRENT_SOURCE_DIR}/ts3510)
set(SrcPath26 ${CMAKE_CURRENT_SOURCE_DIR}/wm8994)

include_directories(${SrcPath0} ${SrcPath1} ${SrcPath2} ${SrcPath3} ${SrcPath4} ${SrcPath5} ${SrcPath6} ${SrcPath7} ${SrcPath8} 
	${SrcPath9} ${SrcPath10} ${SrcPath11} ${SrcPath12} ${SrcPath13} ${SrcPath14} ${SrcPath15} ${SrcPath16} 
	${SrcPath17} ${SrcPath18} ${SrcPath19} ${SrcPath20} ${SrcPath21} ${SrcPath22} ${SrcPath23} ${SrcPath24} 
	${SrcPath25} ${SrcPath26} )

set(SRCS0Path ${SrcPath3})
set(SRCS0 cs43l22.c )
foreach(SRC ${SRCS0})
	list(APPEND SRCS ${SRCS0Path}/${SRC})
endforeach(SRC)

set(SRCS1Path ${SrcPath4})
set(SRCS1 exc7200.c )
foreach(SRC ${SRCS1})
	list(APPEND SRCS ${SRCS1Path}/${SRC})
endforeach(SRC)

set(SRCS2Path ${SrcPath5})
set(SRCS2 ft6x06.c )
foreach(SRC ${SRCS2})
	list(APPEND SRCS ${SRCS2Path}/${SRC})
endforeach(SRC)

set(SRCS3Path ${SrcPath6})
set(SRCS3 ili9325.c )
foreach(SRC ${SRCS3})
	list(APPEND SRCS ${SRCS3Path}/${SRC})
endforeach(SRC)

set(SRCS4Path ${SrcPath7})
set(SRCS4 ili9341.c )
foreach(SRC ${SRCS4})
	list(APPEND SRCS ${SRCS4Path}/${SRC})
endforeach(SRC)

set(SRCS5Path ${SrcPath8})
set(SRCS5 l3gd20.c )
foreach(SRC ${SRCS5})
	list(APPEND SRCS ${SRCS5Path}/${SRC})
endforeach(SRC)

set(SRCS6Path ${SrcPath9})
set(SRCS6 lis302dl.c )
foreach(SRC ${SRCS6})
	list(APPEND SRCS ${SRCS6Path}/${SRC})
endforeach(SRC)

set(SRCS7Path ${SrcPath10})
set(SRCS7 lis3dsh.c )
foreach(SRC ${SRCS7})
	list(APPEND SRCS ${SRCS7Path}/${SRC})
endforeach(SRC)

set(SRCS8Path ${SrcPath11})
set(SRCS8 ls016b8uy.c )
foreach(SRC ${SRCS8})
	list(APPEND SRCS ${SRCS8Path}/${SRC})
endforeach(SRC)

set(SRCS9Path ${SrcPath12})
set(SRCS9 lsm303dlhc.c )
foreach(SRC ${SRCS9})
	list(APPEND SRCS ${SRCS9Path}/${SRC})
endforeach(SRC)

set(SRCS10Path ${SrcPath13})
set(SRCS10 mfxstm32l152.c )
foreach(SRC ${SRCS10})
	list(APPEND SRCS ${SRCS10Path}/${SRC})
endforeach(SRC)

set(SRCS11Path ${SrcPath17})
set(SRCS11 otm8009a.c )
foreach(SRC ${SRCS11})
	list(APPEND SRCS ${SRCS11Path}/${SRC})
endforeach(SRC)

set(SRCS12Path ${SrcPath18})
set(SRCS12 ov2640.c )
foreach(SRC ${SRCS12})
	list(APPEND SRCS ${SRCS12Path}/${SRC})
endforeach(SRC)

set(SRCS13Path ${SrcPath20})
set(SRCS13 s5k5cag.c )
foreach(SRC ${SRCS13})
	list(APPEND SRCS ${SRCS13Path}/${SRC})
endforeach(SRC)

set(SRCS14Path ${SrcPath21})
set(SRCS14 st7735.c )
foreach(SRC ${SRCS14})
	list(APPEND SRCS ${SRCS14Path}/${SRC})
endforeach(SRC)

set(SRCS15Path ${SrcPath22})
set(SRCS15 st7789h2.c )
foreach(SRC ${SRCS15})
	list(APPEND SRCS ${SRCS15Path}/${SRC})
endforeach(SRC)

set(SRCS16Path ${SrcPath23})
set(SRCS16 stmpe1600.c )
foreach(SRC ${SRCS16})
	list(APPEND SRCS ${SRCS16Path}/${SRC})
endforeach(SRC)

set(SRCS17Path ${SrcPath24})
set(SRCS17 stmpe811.c )
foreach(SRC ${SRCS17})
	list(APPEND SRCS ${SRCS17Path}/${SRC})
endforeach(SRC)

set(SRCS18Path ${SrcPath25})
set(SRCS18 ts3510.c )
foreach(SRC ${SRCS18})
	list(APPEND SRCS ${SRCS18Path}/${SRC})
endforeach(SRC)

set(SRCS19Path ${SrcPath26})
set(SRCS19 wm8994.c )
foreach(SRC ${SRCS19})
	list(APPEND SRCS ${SRCS19Path}/${SRC})
endforeach(SRC)


list(SORT SRCS)

add_library(${LibName} OBJECT ${SRCS})
## This library can now be included in any build by using "add_subdirectory"( the_path_to_this_lib ).

