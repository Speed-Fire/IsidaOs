set(STAGE1_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/boot.asm"
)

set(STAGE1_BIN
    "${ISIDA_OUTPUT_DIR}/stage1.bin"
)

add_custom_command(
    OUTPUT "${STAGE1_BIN}"

    COMMAND
        "${NASM_EXECUTABLE}"
        "${STAGE1_SOURCE}"
        -f bin
        -o "${STAGE1_BIN}"

    DEPENDS
        "${STAGE1_SOURCE}"

    COMMENT "Building bootloader stage1"
    VERBATIM
)

add_custom_target(stage1
    DEPENDS "${STAGE1_BIN}"
)

set(STAGE1_BIN "${STAGE1_BIN}" PARENT_SCOPE)