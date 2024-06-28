set(COMPONENT_SRCDIRS
"./src" "./src/extras" "./src/client/SSLClient/client" "./src/client/SSLClient/bssl" 
)

set(COMPONENT_ADD_INCLUDEDIRS
"./src" "./src/extras" "./src/client" "./src/client/SSLClient" "./src/client/SSLClient/client" "./src/client/SSLClient/bssl" 
)


set(COMPONENT_REQUIRES
"arduino"
)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC -DESP32)
target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)  