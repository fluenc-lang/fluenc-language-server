#pragma once

#include "DiagnosticSeverity.h"
#include "Range.h"

namespace lsp
{
	struct Diagnostic
	{
		Range range;
		std::optional<DiagnosticSeverity> severity;
		std::string message;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Diagnostic, range, message)
}
