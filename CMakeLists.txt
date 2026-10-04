cmake_minimum_required(VERSION 3.5)

project(sfilter LANGUAGES C)

EXECUTE_PROCESS(COMMAND uname -r COMMAND tr -d '\n' OUTPUT_VARIABLE KERNEL)

include_directories("/usr/src/linux-headers-${KERNEL}/include")
include_directories("/lib/modules/${KERNEL}/build/include")
#include_directories("/usr/src/linux-headers-${KERNEL}/include/asm-generic/")
#include_directories("/usr/src/linux-headers-${KERNEL}/arch/x86/include/")
#include_directories("/usr/src/linux-headers-${KERNEL}/arch/x86/include/generated/")

set(KDIR "/lib/modules/${KERNEL}/build")
set(KBUILD_CMD make -C ${KDIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules)
set(CLEAN_CMD make -C ${KDIR} M=${CMAKE_CURRENT_SOURCE_DIR} clean)
set(OBJS "nfilter.o filter.o slist.o nlist.o vlist.o nset.o command.o data.o")
file(GLOB SOURCES nfilter.c filter.c slist.c nlist.c vlist.c nset.c command.c data.c)

file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := ${PROJECT_NAME}.o\n")
file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "${PROJECT_NAME}-y := ${OBJS}")

# add_custom_target(kclean ALL
#     COMMAND ${CLEAN_CMD}
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     SOURCES ${SOURCES}
# )

add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${KBUILD_CMD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES ${SOURCES}
)
