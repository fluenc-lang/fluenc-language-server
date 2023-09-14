#pragma once

#include "protocol.h"

namespace lsp
{
	struct Position
	{
		size_t line;
		size_t character;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position
		, line
		, character
	)
}
