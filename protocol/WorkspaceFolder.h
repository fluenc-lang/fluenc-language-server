#pragma once

#include "protocol.h"

namespace lsp
{
	struct WorkspaceFolder
	{
		std::string uri;
		std::string name;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceFolder
		, uri
		, name
	)
}
