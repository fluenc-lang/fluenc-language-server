#pragma once

#include <string>

#include "protocol.h"

namespace lsp
{
	struct ServerInfo
	{
		std::string name;
		std::string version;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerInfo
		, name
		, version
	)
}
