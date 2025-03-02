#pragma once

#include "MarkupKind.h"

namespace lsp
{
	struct MarkupContent
	{
		MarkupKind kind;

		std::string value;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MarkupContent, kind, value)
}
