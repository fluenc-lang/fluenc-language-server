vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO jsonrpcx/json-rpc-cxx
    REF master
    SHA512 f392ee4f48c9f7f1a21928b1c3911901b283b8f7a58cb4695106aa19120587843e9a72ff27b5f516f2f1bc457094cf50b889d096fdd5c4130d218a4ebfac6827
    HEAD_REF 0da30f1
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DCOMPILE_TESTS=OFF
)

vcpkg_cmake_install()
