vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO fluenc-lang/fluenc
    REF master
    SHA512 a08017ecbedbb7131664b5ea482785dbbc6b83022adc1704de5d82d3db7ef9a4d4abec83b062ce8ca177b9047ea398f2d5923b7820d720de7f49303037891af5
    HEAD_REF 5c0444caa4a4e2b3bf2f263fdf6711d04ddbd5ad
    PATCHES
        disable_vcpkg.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(GLOB PUBLIC_HEADERS
  "${SOURCE_PATH}/fluenc-lang/include/*.h"
)

file(COPY ${PUBLIC_HEADERS} DESTINATION "${CURRENT_PACKAGES_DIR}/include/${PORT}")

file(COPY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/fluenc-lang/libfluenc-lang.a" DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
file(COPY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/fluenc-lang/libfluenc-lang.a" DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

file(COPY "${CMAKE_CURRENT_LIST_DIR}/libfluenc-langConfig.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
