#pragma once

#include <string>
#include <vector>

#include "protocol.h"

namespace lsp
{
	struct CompletionOptions
	{
		std::optional<std::vector<std::string>> triggerCharacters;
		std::optional<std::vector<std::string>> allCommitCharacters;
		std::optional<bool> resolveProvider;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CompletionOptions
		, triggerCharacters
		, allCommitCharacters
		, resolveProvider
	)
}
