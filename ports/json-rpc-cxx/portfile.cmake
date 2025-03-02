vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO jsonrpcx/json-rpc-cxx
    REF master
    SHA512 87563a4ca5503258859d15fb5ecb57448bcbea0b5a708d678fc01bc5de0508c4a6b55579bda436aa5c8ca847071ad8b9e982c3ca266638c59213a642c41ab7f6
    HEAD_REF 0da30f1
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DCOMPILE_TESTS=OFF
)

vcpkg_cmake_install()
