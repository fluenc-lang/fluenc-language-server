#pragma once

#include "protocol.h"

namespace lsp
{
	enum class PositionEncodingKind
	{
		UTF8,
		UTF16,
		UTF32
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(PositionEncodingKind, {
		{ PositionEncodingKind::UTF8, "utf-8" },
		{ PositionEncodingKind::UTF16, "utf-16" },
		{ PositionEncodingKind::UTF32, "utf-32" } }
	)
}
