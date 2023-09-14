add_library(libfluenc-lang INTERFACE)

find_library(LIB "fluenc-lang")

find_package(LLVM REQUIRED CONFIG)
find_package(Immer CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

target_link_libraries(libfluenc-lang INTERFACE
    ${LIB}

    immer
    range-v3

    fmt::fmt
)

if (${LLVM_LINK_LLVM_DYLIB})
	target_link_libraries(libfluenc-lang INTERFACE
		LLVM
	)
else()
	target_link_libraries(libfluenc-lang INTERFACE
		${LLVM_AVAILABLE_LIBS}
	)
endif()

target_link_directories(libfluenc-lang INTERFACE
    ${LLVM_LIBRARY_DIR}
)
