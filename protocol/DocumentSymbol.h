#pragma once

#include <string>

#include "SymbolKind.h"
#include "Range.h"

namespace lsp
{
    struct DocumentSymbol
    {
        std::string name;

        SymbolKind kind;

		Range range;
		Range selectionRange;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DocumentSymbol
		, name
		, kind
		, range
		, selectionRange
	)
}
