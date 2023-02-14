## Boombaard generated CMakelists add_library file
## File generated on : 2018-10-28 12:51:25.
## (c)2002-2017 Kiwanda Embedded Systemen.
## $Id: CMakeLists.txt 6931 2017-02-10 19:46:52Z ewout $

# The absolute path for this CMakeLists.txt file is : "D:\swexternals\embedded\Segger\Segger_RTT"
set(LibName Segger_RTT)

set(SrcPath0 ${CMAKE_CURRENT_SOURCE_DIR}/h)
set(SrcPath1 ${CMAKE_CURRENT_SOURCE_DIR}/RTT)
set(SrcPath2 ${CMAKE_CURRENT_SOURCE_DIR}/.)
set(SrcPath3 ${CMAKE_CURRENT_SOURCE_DIR}/Syscalls)

set(AllIncludeDirs ${SrcPath0} ${SrcPath1} )
include_directories(${AllIncludeDirs})

set(SRCS )

set(SRCS0Path ${SrcPath1})
set(SRCS0 SEGGER_RTT.c SEGGER_RTT_printf.c )
foreach(SRC ${SRCS0})
	list(APPEND SRCS ${SRCS0Path}/${SRC})
endforeach(SRC)

foreach(SRC ${SRCS1})
	list(APPEND SRCS ${SRCS1Path}/${SRC})
endforeach(SRC)

set(SRCS2Path ${SrcPath3})
set(SRCS2 SEGGER_RTT_Syscalls_GCC.c SEGGER_RTT_Syscalls_IAR.c SEGGER_RTT_Syscalls_KEIL.c SEGGER_RTT_Syscalls_SES.c)

foreach(SRC ${SRCS2})
	list(APPEND SRCS ${SRCS2Path}/${SRC})
endforeach(SRC)


list(SORT SRCS)

MaakLib(${LibName} "${SRCS}")

set(LibInclude ${AllIncludeDirs})


