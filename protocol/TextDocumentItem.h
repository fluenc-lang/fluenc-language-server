#pragma once

#include <string>

#include "protocol.h"

namespace lsp
{
	struct TextDocumentItem
	{
		std::string uri;
		std::string languageId;
		std::string text;

		int version;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentItem
		, uri
		, languageId
		, text
		, version
	)
}
