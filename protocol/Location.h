#pragma once

#include "Range.h"

namespace lsp
{
	struct Location
	{
		std::string uri;

		Range range;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Location, uri, range)
}
