#pragma once

#include <string>

#include "protocol.h"

namespace lsp
{
	struct TextDocumentIdentifier
	{
		std::string uri;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentIdentifier, uri)
}
