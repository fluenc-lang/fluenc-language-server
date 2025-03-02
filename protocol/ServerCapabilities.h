#pragma once

#include "TextDocumentSyncOptions.h"
#include "PositionEncodingKind.h"
#include "CompletionOptions.h"

namespace lsp
{
	struct ServerCapabilities
	{
		std::optional<PositionEncodingKind> positionEncoding;
		std::optional<TextDocumentSyncOptions> textDocumentSync;
		std::optional<CompletionOptions> completionProvider;
		std::optional<bool> hoverProvider;
		std::optional<bool> documentHighlightProvider;
		std::optional<bool> documentSymbolProvider;
		std::optional<bool> referencesProvider;
		std::optional<bool> declarationProvider;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerCapabilities
		, positionEncoding
		, textDocumentSync
		, completionProvider
		, hoverProvider
		, documentHighlightProvider
		, documentSymbolProvider
		, referencesProvider
		, declarationProvider
	)
}
