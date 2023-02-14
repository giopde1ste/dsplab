## Copyright 2011-2016 Hogeschool van Arnhem en Nijmegen
## $Id: CMakeLists.txt 298 2022-02-24 14:43:35Z ewout $

# voeg opdracht1 toe aan de bronbestanden
include_directories(${firfilterdir} ${OPDRACHT1DIR}/STUDENT/h)
add_subdirectory(${OPDRACHT1DIR} ${CMAKE_CURRENT_BINARY_DIR}/Opdracht1)

# voeg opdracht 4 toe aan de bronbestanden
set(firfilterdir ${OPDRACHT4DIR}/STUDENT/filter)


add_subdirectory(${firfilterdir} ${CMAKE_CURRENT_BINARY_DIR}/filter)


include_directories(h)

set(SRCS ../Basis/main.cpp student.cpp $<TARGET_OBJECTS:DSBKlassen>)
list(SORT SRCS)

# Doel wordt hier samengesteld.
add_executable(${PROJEKTNAAM}.elf ${SRCS} ${BasisSRCS})

target_link_libraries(${PROJEKTNAAM}.elf FirFilterLib ${ESE_BSPLib} ${F412GDiscoveryBibliotheken} )

PostBuildAktie(${PROJEKTNAAM}.elf "STM32 ${PROJEKTNAAM}.elf is gemaakt.")


