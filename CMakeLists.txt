# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(Sinecraft)

include(GenerateG1A)
include(GenerateG3A)
include(Fxconv)
find_package(Gint 2.10 REQUIRED)

set(SOURCES
    src/serial.h
    src/serial.c
    src/keys.h
    src/keys.c
    src/ui.c
    src/ui.h
    src/main.c
    # ...
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
    # ...
)
set(ASSETS_fx
    # ...
)
set(ASSETS_cg
    # assets-cg/logo.png
    # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(fxnet ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(fxnet PRIVATE -Wall -Wextra -Os)
target_link_libraries(fxnet Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
    generate_g1a(
        TARGET fxnet OUTPUT "fxNET.g1a"
        NAME "fxNET" ICON assets-fx/icon.png
    )
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
    generate_g3a(
        TARGET fxnet OUTPUT "fxNET.g3a"
        NAME "fxNET" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png
    )
endif()
