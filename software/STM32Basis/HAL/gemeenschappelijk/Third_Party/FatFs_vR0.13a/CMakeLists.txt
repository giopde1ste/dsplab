## Boombaard generated CMakelists add_library file
## File generated on : 2018-02-27 15:51:31.

# The absolute path for this CMakeLists.txt file is : "C:\Users\ewout\home\vakken\algemeen\software\STM32\STM32Basis\HAL\gemeenschappelijk\Third_Party\FatFs_vR0.13a"
set(LibName FatFs_vR0.13a)

set(SrcPath0 ${CMAKE_CURRENT_SOURCE_DIR}/.)

set(AllIncludeDirs . )
include_directories(${AllIncludeDirs})

set(SRCS ff.c ffsystem.c ffunicode.c )

list(SORT SRCS)

add_library(${LibName} STATIC ${SRCS})
## This library can now be included in any build by using "add_subdirectory( the_path_to_this_lib )".