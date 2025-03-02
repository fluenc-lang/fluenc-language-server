vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO fluenc-lang/fluenc
    REF 76042231520c5d9b0bbcb12aad9c798b479844cb
    SHA512 876e5669826301a5afe22bae2047bfaf4d659255fc0d25f5ba7e138210aa30ea76316424126a698e1bf810a980166cc5fabf341f6b24014a722e8035409fe756
    PATCHES
        disable_vcpkg.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/fluenc)

# file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
#
# file(GLOB PUBLIC_HEADERS
#   "${SOURCE_PATH}/fluenc-lang/include/*.h"
# )
#
# file(COPY ${PUBLIC_HEADERS} DESTINATION "${CURRENT_PACKAGES_DIR}/include/${PORT}")
#
# file(COPY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/fluenc-lang/libfluenc-lang.a" DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
# file(COPY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/fluenc-lang/libfluenc-lang.a" DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
#
# file(COPY "${CMAKE_CURRENT_LIST_DIR}/libfluenc-langConfig.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
