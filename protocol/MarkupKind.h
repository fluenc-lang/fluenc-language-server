#pragma once

#include "protocol.h"

namespace lsp
{
	enum class MarkupKind
	{
		PlainText,
		Markdown
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(
		MarkupKind,
		{
			{ MarkupKind::PlainText, "plaintext" },
			{ MarkupKind::Markdown, "markdown" },
		}
	)
}
