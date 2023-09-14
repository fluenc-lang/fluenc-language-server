#pragma once

#include "Position.h"

namespace lsp
{
	struct Range
	{
		Position start;
		Position end;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Range, start, end)
}
