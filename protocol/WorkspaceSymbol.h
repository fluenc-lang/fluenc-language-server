#pragma once

#include "SymbolKind.h"
#include "Location.h"

namespace lsp
{
	struct WorkspaceSymbol
	{
		std::string name;

		SymbolKind kind;
		Location location;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceSymbol
		, name
		, kind
		, location
	)
}
