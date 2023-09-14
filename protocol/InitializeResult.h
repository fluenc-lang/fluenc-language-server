#pragma once

#include "ServerCapabilities.h"
#include "ServerInfo.h"

namespace lsp
{
	struct InitializeResult
	{
		ServerCapabilities capabilities;
		ServerInfo serverInfo;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InitializeResult
		, capabilities
		, serverInfo
	)
}
