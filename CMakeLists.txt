cmake_minimum_required(VERSION 3.14)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.26.1.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

run_vcpkg(
	VCPKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fluenc-language-server LANGUAGES C CXX)

find_package(libfluenc-lang CONFIG REQUIRED)

find_path(JSON_RPC_CXX_INCLUDE_DIRS "jsonrpccxx/batchclient.hpp")

add_executable(${PROJECT_NAME}
	main.cpp

	protocol/protocol.h
	protocol/WorkspaceCapabilities.h
	protocol/ClientCapabilities.h
	protocol/TextDocumentSyncKind.h
	protocol/TextDocumentSyncOptions.h
	protocol/ServerCapabilities.h
	protocol/PositionEncodingKind.h
	protocol/CompletionOptions.h
	protocol/SymbolKind.h
	protocol/DocumentSymbol.h
	protocol/Position.h
	protocol/ServerInfo.h
	protocol/InitializeResult.h
	protocol/TextDocumentIdentifier.h
	protocol/Range.h
	protocol/Location.h
	protocol/WorkspaceSymbol.h
	protocol/TextDocumentItem.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${JSON_RPC_CXX_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
	libfluenc-lang
)

if (MSVC)
	add_compile_options(/Zc:__cplusplus /bigobj)
	add_compile_definitions(NOMINMAX)
	add_compile_definitions(NDEBUG)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_custom_target(vcpkg SOURCES
	vcpkg.json
)
