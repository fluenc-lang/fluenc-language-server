#pragma once

#include <string>

#include "SymbolKind.h"

namespace lsp
{
    struct DocumentSymbol
    {
        std::string name;

        SymbolKind kind;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DocumentSymbol
		, name
		, kind
	)
}
