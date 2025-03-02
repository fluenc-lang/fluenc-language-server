#pragma once

#include <string>

#include "protocol.h"

namespace lsp
{
	struct TextDocumentContentChangeEvent
	{
		std::string text;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentContentChangeEvent
		, text
		)
}
