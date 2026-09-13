cmake_minimum_required (VERSION 3.20)
project ("IsidaOs" LANGUAGES C)

# ------------------------------------------------------------
# Directories
# ------------------------------------------------------------

set(SRC_DIR   "${CMAKE_SOURCE_DIR}/src")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools")

# Не привязываемся к исходникам.
# CMAKE_BINARY_DIR будет, например:
#
#   out/build/wsl-debug
#
set(BUILD_DIR "${CMAKE_BINARY_DIR}")
set(ISIDA_OUTPUT_DIR "${CMAKE_BINARY_DIR}")

file(MAKE_DIRECTORY "${BUILD_DIR}")

# ------------------------------------------------------------
# External programs
# ------------------------------------------------------------

find_program(NASM_EXECUTABLE nasm REQUIRED)
find_program(IA16_GCC_EXECUTABLE 
             NAMES ia16-elf-gcc
             HINTS
                "/usr/local/cross/bin"
                "/opt/ia16/bin"
             REQUIRED)
find_program(IA16_LD_EXECUTABLE
             NAMES ia16-elf-ld
             HINTS
                "/usr/local/cross/bin"
                "/opt/ia16/bin"
             REQUIRED)

find_program(DD_EXECUTABLE dd REQUIRED)
find_program(MKFS_FAT_EXECUTABLE mkfs.fat REQUIRED)
find_program(MCOPY_EXECUTABLE mcopy REQUIRED)

# ------------------------------------------------------------
# Components
# ------------------------------------------------------------

add_subdirectory(src/bootloader/stage1)
add_subdirectory(src/bootloader/stage2)
add_subdirectory(src/kernel)

# ============================================================
# FAT tool
# ============================================================

add_executable(fat
    tools/fat/fat.c
)

set_target_properties(fat PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES

    RUNTIME_OUTPUT_DIRECTORY "${ISIDA_OUTPUT_DIR}/tools"
)

# ------------------------------------------------------------
# Floppy image
# ------------------------------------------------------------

set(FLOPPY_IMAGE "${ISIDA_OUTPUT_DIR}/main_floppy.img")

add_custom_command(
    OUTPUT "${FLOPPY_IMAGE}"

    COMMAND
        "${DD_EXECUTABLE}"
        if=/dev/zero
        of=${FLOPPY_IMAGE}
        bs=512
        count=2880

    COMMAND
        "${MKFS_FAT_EXECUTABLE}"
        -F 12
        -n NBOS
        "${FLOPPY_IMAGE}"

    COMMAND
        "${DD_EXECUTABLE}"
        if=${STAGE1_BIN}
        of=${FLOPPY_IMAGE}
        conv=notrunc

    COMMAND
        "${MCOPY_EXECUTABLE}"
        -i "${FLOPPY_IMAGE}"
        "${STAGE2_BIN}"
        "::stage2.bin"

    COMMAND
        "${MCOPY_EXECUTABLE}"
        -i "${FLOPPY_IMAGE}"
        "${KERNEL_BIN}"
        "::kernel.bin"

    COMMAND
        "${MCOPY_EXECUTABLE}"
        -i "${FLOPPY_IMAGE}"
        "${CMAKE_SOURCE_DIR}/test.txt"
        "::test.txt"

    DEPENDS
        "${STAGE1_BIN}"
        "${STAGE2_BIN}"
        "${KERNEL_BIN}"
        "${CMAKE_SOURCE_DIR}/test.txt"

    COMMENT "Creating Isida floppy image"
    VERBATIM
)

add_custom_target(floppy_image ALL
    DEPENDS "${FLOPPY_IMAGE}"
)

add_dependencies(
    floppy_image
    stage1
    stage2
    kernel
)

# ============================================================
# Dummy executable for launching Os in Visual Studio
# ============================================================

add_executable(isida_run
    tools/launcher.c
)

add_dependencies(isida_run floppy_image)

# ============================================================
# Default "all"
# ============================================================

#add_custom_target(IsidaOs ALL)
#
#add_dependencies(
#    IsidaOs
#    floppy_image
#    fat
#)