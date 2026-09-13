set(STAGE2_BUILD_DIR
    "${ISIDA_OUTPUT_DIR}/stage2"
)

set(STAGE2_C_BUILD_DIR
    "${STAGE2_BUILD_DIR}/c"
)

set(STAGE2_ASM_BUILD_DIR
    "${STAGE2_BUILD_DIR}/asm"
)

file(MAKE_DIRECTORY
    "${STAGE2_C_BUILD_DIR}"
    "${STAGE2_ASM_BUILD_DIR}"
)


# ------------------------------------------------------------
# Sources
# ------------------------------------------------------------

file(GLOB STAGE2_C_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

file(GLOB STAGE2_ASM_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.asm"
)


# ------------------------------------------------------------
# ia16 GCC flags
# ------------------------------------------------------------

set(STAGE2_CFLAGS
    -ffreestanding
    -march=i8086
    -mcmodel=small
    -fno-stack-protector
    -fno-pic
    -fno-pie
    -fno-builtin
    -fno-asynchronous-unwind-tables
    -fno-unwind-tables
    -Wall
    -nostdlib
)


# ------------------------------------------------------------
# Compile C sources
# ------------------------------------------------------------

set(STAGE2_C_OBJECTS)

foreach(SOURCE_FILE IN LISTS STAGE2_C_SOURCES)

    get_filename_component(
        SOURCE_NAME
        "${SOURCE_FILE}"
        NAME_WE
    )

    set(OBJECT_FILE
        "${STAGE2_C_BUILD_DIR}/${SOURCE_NAME}.obj"
    )

    set(DEP_FILE
    "${STAGE2_C_BUILD_DIR}/${SOURCE_NAME}.d"
    )

    add_custom_command(
        OUTPUT "${OBJECT_FILE}"

        COMMAND
            "${IA16_GCC_EXECUTABLE}"
            ${STAGE2_CFLAGS}
            -MMD
            -MP
            -MF "${DEP_FILE}"
            -c "${SOURCE_FILE}"
            -o "${OBJECT_FILE}"

        DEPFILE
            "${DEP_FILE}"

        DEPENDS
            "${SOURCE_FILE}"

        COMMENT
            "IA16 CC ${SOURCE_NAME}.c"

        VERBATIM
    )

    list(APPEND STAGE2_C_OBJECTS
        "${OBJECT_FILE}"
    )

endforeach()


# ------------------------------------------------------------
# Assemble ASM sources
# ------------------------------------------------------------

set(STAGE2_ASM_OBJECTS)

foreach(SOURCE_FILE IN LISTS STAGE2_ASM_SOURCES)

    get_filename_component(
        SOURCE_NAME
        "${SOURCE_FILE}"
        NAME_WE
    )

    set(OBJECT_FILE
        "${STAGE2_ASM_BUILD_DIR}/${SOURCE_NAME}.obj"
    )

    add_custom_command(
        OUTPUT "${OBJECT_FILE}"

        COMMAND
            "${NASM_EXECUTABLE}"
            -f elf32
            -o "${OBJECT_FILE}"
            "${SOURCE_FILE}"

        DEPENDS
            "${SOURCE_FILE}"

        COMMENT
            "NASM ${SOURCE_NAME}.asm"

        VERBATIM
    )

    list(APPEND STAGE2_ASM_OBJECTS
        "${OBJECT_FILE}"
    )

endforeach()


# ------------------------------------------------------------
# Link stage2
# ------------------------------------------------------------

set(STAGE2_BIN
    "${ISIDA_OUTPUT_DIR}/stage2.bin"
)

set(STAGE2_MAP
    "${ISIDA_OUTPUT_DIR}/stage2.map"
)

set(STAGE2_LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

add_custom_command(
    OUTPUT
        "${STAGE2_BIN}"

    BYPRODUCTS
        "${STAGE2_MAP}"

    COMMAND
        "${IA16_LD_EXECUTABLE}"
        -T "${STAGE2_LINKER_SCRIPT}"
        "-Map=${STAGE2_MAP}"
        -o "${STAGE2_BIN}"
        ${STAGE2_ASM_OBJECTS}
        ${STAGE2_C_OBJECTS}

    DEPENDS
        ${STAGE2_ASM_OBJECTS}
        ${STAGE2_C_OBJECTS}
        "${STAGE2_LINKER_SCRIPT}"

    COMMENT
        "Linking stage2.bin"

    VERBATIM
)


add_custom_target(stage2
    DEPENDS "${STAGE2_BIN}"
)

set(STAGE2_BIN "${STAGE2_BIN}" PARENT_SCOPE)