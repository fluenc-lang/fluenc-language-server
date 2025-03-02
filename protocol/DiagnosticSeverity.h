#pragma once

#include "protocol.h"

namespace lsp
{
	enum class DiagnosticSeverity
	{
		Error,
		Warning,
		Information,
		Hint
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(
		DiagnosticSeverity,
		{ { DiagnosticSeverity::Error, "Error" },
		  { DiagnosticSeverity::Warning, "Warning" },
		  { DiagnosticSeverity::Information, "Information" },
		  { DiagnosticSeverity::Hint, "Hint" } }
	)
}
