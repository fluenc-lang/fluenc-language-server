#pragma once

#include "WorkspaceCapabilities.h"

namespace lsp
{
	struct ClientCapabilities
	{
		std::optional<WorkspaceCapabilities> workspace;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientCapabilities
		, workspace
	)
}
