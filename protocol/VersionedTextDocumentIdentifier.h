#pragma once

#include "TextDocumentIdentifier.h"

namespace lsp
{
	struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier
	{
		size_t version;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VersionedTextDocumentIdentifier
		, uri
		, version
		)
}
