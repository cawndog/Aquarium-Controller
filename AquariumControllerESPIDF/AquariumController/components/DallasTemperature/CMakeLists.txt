set(COMPONENT_SRCDIRS
"./src"
)

set(COMPONENT_ADD_INCLUDEDIRS
"./"
)

set(COMPONENT_REQUIRES
"arduino" "OneWire"
)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC -DESP32)
target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)