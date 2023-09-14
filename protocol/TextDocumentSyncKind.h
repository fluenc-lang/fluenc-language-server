#pragma once

#include "protocol.h"

namespace lsp
{
	enum class TextDocumentSyncKind
	{
		None = 0,
		Full = 1,
		Incremental = 2,
	};
}
