#pragma once

#include <peglib.h>

#include <fluenc-lang/ast.hpp>
#include <fluenc-lang/ast/module_node.hpp>
#include <fluenc-lang/expression.hpp>

#include <iostream>
#include <unordered_map>

#include "protocol/Location.h"

struct location_resolver
{
	using return_type = std::optional<lsp::Location>;
	using context_type = std::monostate;

	location_resolver(const std::unordered_map<std::string, fluenc::module_node>& documents)
		: documents_(documents)
	{
	}

	std::optional<lsp::Location> visit(const fluenc::binary_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::integral_literal_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::float_literal_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::boolean_literal_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::string_literal_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::character_literal_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::nothing_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::member_access_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::function_call_node* node, std::monostate context) const
	{
		for (auto& [uri, module] : documents_)
		{
			auto function_it = std::ranges::find_if(module.functions, [node](auto pair) -> bool {
				return std::ranges::any_of(node->names, std::bind_front(std::equal_to {}, pair.first));
			});

			if (function_it == end(module.functions))
			{
				continue;
			}

			auto& [_, function] = *function_it;

			return lsp::Location {
				.uri = uri,
				.range = {
					.start = {
						.line = function->ast->line - 1,
						.character = function->ast->column - 1,
					},
					.end = {
						.line = function->ast->line,
						.character = function->ast->column + function->ast->length - 1,
					},
				},
			};
		}

		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::instantiation_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::conditional_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::block_instruction_node* node, std::monostate context) const
	{
		return accept(node->subject, *this, context);
	}

	std::optional<lsp::Location> visit(const fluenc::array_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::expansion_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::local_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::unary_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::function_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::global_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::return_node* node, std::monostate context) const
	{
		return accept(node->consumer, *this, context);
	}

	std::optional<lsp::Location> visit(const fluenc::terminator_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::tuple_sink_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::struct_node* node, std::monostate context) const
	{
		return {};
	}

	std::optional<lsp::Location> visit(const fluenc::module_node& node, std::monostate context) const
	{
		for (auto& [_, function] : node.functions)
		{
			if (auto location = accept(function, *this, context))
			{
				return location;
			}
		}

		for (auto& [_, global] : node.globals)
		{
			if (auto location = accept(global, *this, context))
			{
				return location;
			}
		}

		for (auto& [_, type] : node.types)
		{
			if (auto location = accept(type, *this, context))
			{
				return location;
			}
		}

		return {};
	}

private:
	std::unordered_map<std::string, fluenc::module_node> documents_;
};
