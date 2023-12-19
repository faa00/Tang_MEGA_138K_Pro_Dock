cmake_minimum_required(VERSION 3.20)

project(ae350_demo)

enable_language(C CXX ASM)

set(SDK_TCH gcc)
set(SDK_LIB newlib)

#set(SDK_TCH clang)
#set(SDK_LIB mculib)
#set(SDK_LIB newlib)

set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

set(tools /home/faa/toolchain/ae350-${SDK_TCH}/nds32le-elf-${SDK_LIB}-v5d)

set(RISC_ABI ilp32d)

set(RISCV_TOOLCHAIN ${tools}/bin/riscv32-elf-)

set(CMAKE_SYSROOT ${tools}/riscv32-elf)

set(AE350_GOWIN_SDK ${CMAKE_CURRENT_LIST_DIR}/src)

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

add_compile_options( -Og -g3 -mcpu=a25 -mabi=${RISC_ABI} -mcmodel=medium)
add_compile_options( -ffunction-sections -fdata-sections)
## -fpack-struct -fshort-enums)
add_compile_options( -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing)
add_compile_options( -Wall)
#add_compile_options(-Wall -Wno-nonnull-compare -Wextra -Wshadow -Werror -Wno-gnu-zero-variadic-macro-arguments)

set(LINKER_SCRIPT "ae350-ddr.ld")
#
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Og -nostartfiles -static -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${AE350_GOWIN_SDK}/bsp/sag/${LINKER_SCRIPT}")

# link_libraries(-lstdc++)

aux_source_directory(${AE350_GOWIN_SDK}/bsp/ae350 SRCS_COMMON)
aux_source_directory(${AE350_GOWIN_SDK}/bsp/driver/ae350 SRCS_DRIVERS)
aux_source_directory(${AE350_GOWIN_SDK}/bsp/driver/include SRCS_DRIVERS_INC)
aux_source_directory(${AE350_GOWIN_SDK}/bsp/config SRCS_CONFIG)
aux_source_directory(${AE350_GOWIN_SDK}/bsp/lib SRCS_LIB)
aux_source_directory(${AE350_GOWIN_SDK}/demo SRCS_APP)
aux_source_directory(${AE350_GOWIN_SDK}/demo/cache      SRCS_APP_cache)
aux_source_directory(${AE350_GOWIN_SDK}/demo/cache_lock SRCS_APP_cache_lock)
aux_source_directory(${AE350_GOWIN_SDK}/demo/ddr3       SRCS_APP_ddr3)
aux_source_directory(${AE350_GOWIN_SDK}/demo/gpio       SRCS_APP_gpio)
aux_source_directory(${AE350_GOWIN_SDK}/demo/hsp        SRCS_APP_hsp)
aux_source_directory(${AE350_GOWIN_SDK}/demo/i2c        SRCS_APP_i2c)
aux_source_directory(${AE350_GOWIN_SDK}/demo/intr       SRCS_APP_intr)
aux_source_directory(${AE350_GOWIN_SDK}/demo/led        SRCS_APP_led)
aux_source_directory(${AE350_GOWIN_SDK}/demo/mm         SRCS_APP_mm)
aux_source_directory(${AE350_GOWIN_SDK}/demo/pfm        SRCS_APP_pfm)
aux_source_directory(${AE350_GOWIN_SDK}/demo/plic       SRCS_APP_plic)
aux_source_directory(${AE350_GOWIN_SDK}/demo/plmt       SRCS_APP_plmt)
aux_source_directory(${AE350_GOWIN_SDK}/demo/powerbrake SRCS_APP_powerbrake)
aux_source_directory(${AE350_GOWIN_SDK}/demo/printf     SRCS_APP_printf)
aux_source_directory(${AE350_GOWIN_SDK}/demo/pwm        SRCS_APP_pwm)
aux_source_directory(${AE350_GOWIN_SDK}/demo/rtc        SRCS_APP_rtc)
aux_source_directory(${AE350_GOWIN_SDK}/demo/scanf      SRCS_APP_scanf)
aux_source_directory(${AE350_GOWIN_SDK}/demo/slaveport  SRCS_APP_slaveport)
aux_source_directory(${AE350_GOWIN_SDK}/demo/spi        SRCS_APP_spi)
aux_source_directory(${AE350_GOWIN_SDK}/demo/uart       SRCS_APP_uart)
aux_source_directory(${AE350_GOWIN_SDK}/demo/wdt_pit    SRCS_APP_wdt_pit)
aux_source_directory(${AE350_GOWIN_SDK}/demo/wfi        SRCS_APP_wfi)


# file(GLOB_RECURSE SRC_LIST "${AE350_GOWIN_SDK}/demo" "*.c*")

# set(SRCS_APP ${AE350_GOWIN_SDK}/demo)

set(SRCS
    ${SRCS_COMMON}
    ${SRCS_DRIVERS}
    #${SRCS_DRIVERS_INC}
    ${SRCS_CONFIG}
    ${SRCS_LIB}

    ${SRCS_APP_cache}
    ${SRCS_APP_cache_lock}
    ${SRCS_APP_ddr3}
    ${SRCS_APP_gpio}
    ${SRCS_APP_hsp}
    ${SRCS_APP_i2c}
    ${SRCS_APP_intr}
    ${SRCS_APP_led}
    ${SRCS_APP_mm}
    ${SRCS_APP_pfm}
    ${SRCS_APP_plic}
    ${SRCS_APP_plmt}
    ${SRCS_APP_powerbrake}
    ${SRCS_APP_printf}
    ${SRCS_APP_pwm}
    ${SRCS_APP_rtc}
    ${SRCS_APP_scanf}
    ${SRCS_APP_slaveport}
    ${SRCS_APP_spi}
    ${SRCS_APP_uart}
    ${SRCS_APP_wdt_pit}
    ${SRCS_APP_wfi}

    ${SRCS_APP}
    ${AE350_GOWIN_SDK}/bsp/ae350/start.S
)

# include_directories(${CMAKE_CURRENT_LIST_DIR})

include_directories(${AE350_GOWIN_SDK}/bsp/ae350)
include_directories(${AE350_GOWIN_SDK}/bsp/config)
include_directories(${AE350_GOWIN_SDK}/bsp/driver/ae350)
include_directories(${AE350_GOWIN_SDK}/bsp/driver/include)
include_directories(${AE350_GOWIN_SDK}/bsp/lib)
include_directories(${AE350_GOWIN_SDK}/demo)

# link_directories(${AE350_GOWIN_SDK}/driver/ae350)

add_executable(${PROJECT_NAME}.elf ${SRCS})

add_custom_target(BIN ALL
COMMAND ${OBJCOPY} -S -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
COMMAND ${SIZE} ${PROJECT_NAME}.elf
COMMAND ${OBJDUMP} -x -d -C ${PROJECT_NAME}.elf > objdump.txt
COMMAND ${NM} -n -l -C ${PROJECT_NAME}.elf > symbol.txt
COMMAND ${READELF} -a ${PROJECT_NAME}.elf > readelf.txt
DEPENDS ${PROJECT_NAME}.elf
)
