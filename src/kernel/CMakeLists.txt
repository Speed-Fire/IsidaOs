set(KERNEL_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/kernel.asm"
)

set(KERNEL_BIN
    "${ISIDA_OUTPUT_DIR}/kernel.bin"
)

add_custom_command(
    OUTPUT "${KERNEL_BIN}"

    COMMAND
        "${NASM_EXECUTABLE}"
        "${KERNEL_SOURCE}"
        -f bin
        -o "${KERNEL_BIN}"

    DEPENDS
        "${KERNEL_SOURCE}"

    COMMENT "Building kernel"
    VERBATIM
)

add_custom_target(kernel
    DEPENDS "${KERNEL_BIN}"
)

set(KERNEL_BIN "${KERNEL_BIN}" PARENT_SCOPE)