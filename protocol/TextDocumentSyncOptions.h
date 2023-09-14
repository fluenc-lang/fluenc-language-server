#pragma once

#include "TextDocumentSyncKind.h"

namespace lsp
{
	struct TextDocumentSyncOptions
	{
		std::optional<bool> openClose;
		std::optional<TextDocumentSyncKind> change;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentSyncOptions
		, openClose
		, change
	)
}
