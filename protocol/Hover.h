#pragma once

#include "MarkupContent.h"
#include "Range.h"

namespace lsp
{
	struct Hover
	{
		MarkupContent contents;

		std::optional<Range> range;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Hover, contents, range)
}
