cmake_minimum_required(VERSION 3.20)

project(ae350_dhrystone)

enable_language(C CXX ASM)

#set(SDK_TCH gcc)
#set(SDK_LIB newlib)

set(SDK_TCH clang)
set(SDK_LIB mculib)
#set(SDK_LIB newlib)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

set(tools /home/faa/toolchain/ae350-${SDK_TCH}/nds32le-elf-${SDK_LIB}-v5d)

set(RISC_ABI ilp32d)

set(RISCV_TOOLCHAIN ${tools}/bin/riscv32-elf-)

set(CMAKE_SYSROOT ${tools}/riscv32-elf)

set(AE350_PROG ${CMAKE_CURRENT_LIST_DIR}/src)

# set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER ${RISCV_TOOLCHAIN}gcc)
set(CMAKE_CXX_COMPILER ${RISCV_TOOLCHAIN}g++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(OBJCOPY ${RISCV_TOOLCHAIN}objcopy)
set(OBJDUMP ${RISCV_TOOLCHAIN}objdump)
set(NM ${RISCV_TOOLCHAIN}nm)
set(READELF ${RISCV_TOOLCHAIN}readelf)
set(SIZE ${RISCV_TOOLCHAIN}size)

SET(CMAKE_ASM_FLAGS "${CFLAGS} -x assembler-with-cpp")

add_compile_options(-DTIME)
add_compile_options(-O3 -g3 -mcpu=a25 -mabi=${RISC_ABI} -mcmodel=medium -mext-dsp)
add_compile_options(-ffunction-sections -fdata-sections -c)
## -fpack-struct -fshort-enums)
add_compile_options(-fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing)
add_compile_options(-Wall)
#add_compile_options(-Wall -Wno-nonnull-compare -Wextra -Wshadow -Werror -Wno-gnu-zero-variadic-macro-arguments)

set(LINKER_SCRIPT "ae350-ddr.ld")
#
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -nostartfiles -static -mext-dsp -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${AE350_PROG}/bsp/sag/${LINKER_SCRIPT}")

link_libraries(-ldsp -lm)

aux_source_directory(${AE350_PROG}/bsp/ae350 SRCS_COMMON)
aux_source_directory(${AE350_PROG}/bsp/driver/ae350 SRCS_DRIVERS)
aux_source_directory(${AE350_PROG}/bsp/driver/include SRCS_DRIVERS_INC)
aux_source_directory(${AE350_PROG}/bsp/config SRCS_CONFIG)
aux_source_directory(${AE350_PROG}/bsp/lib SRCS_LIB)
aux_source_directory(${AE350_PROG}/demo SRCS_APP)
aux_source_directory(${AE350_PROG}/demo/stubs SRCS_APP_STUBS)
# aux_source_directory(${AE350_PROG}/demo/gpio SRCS_APP_GPIO)

# file(GLOB_RECURSE SRC_LIST "${SRCS_APP}/" "*.c*")

# set(SRCS_APP ${AE350_PROG}/demo)

    set(SRCS
    ${SRCS_COMMON}
    ${SRCS_DRIVERS}
    #${SRCS_DRIVERS_INC}
    ${SRCS_CONFIG}
    ${SRCS_LIB}
    ${SRCS_APP_STUBS}
#    ${SRCS_APP_GPIO}
    ${SRCS_APP}
#    ${SRC_LIST}
    ${AE350_PROG}/bsp/ae350/start.S

)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${AE350_PROG}/bsp/ae350)
include_directories(${AE350_PROG}/bsp/config)
include_directories(${AE350_PROG}/bsp/driver/ae350)
include_directories(${AE350_PROG}/bsp/driver/include)
include_directories(${AE350_PROG}/bsp/lib)
include_directories(${AE350_PROG}/demo/stubs)
include_directories(${AE350_PROG}/demo)

# link_directories(${AE350_PROG}/driver/ae350)

add_executable(${PROJECT_NAME}.elf ${SRCS})

add_custom_target(BIN ALL
COMMAND ${OBJCOPY} -S -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
COMMAND ${SIZE} ${PROJECT_NAME}.elf
COMMAND ${OBJDUMP} -x -d -C ${PROJECT_NAME}.elf > objdump.txt
COMMAND ${NM} -n -l -C ${PROJECT_NAME}.elf > symbol.txt
COMMAND ${READELF} -a ${PROJECT_NAME}.elf > readelf.txt
DEPENDS ${PROJECT_NAME}.elf
)
