#pragma once

#include <fluenc-lang/ast.hpp>
#include <fluenc-lang/ast/module_node.hpp>
#include <fluenc-lang/expression.hpp>

#include <ranges>
#include <unordered_map>

struct ast_extractor
{
	using return_type = std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t>;
	using context_type = std::monostate;

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::binary_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::integral_literal_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::float_literal_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::boolean_literal_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::string_literal_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::character_literal_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::nothing_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::member_access_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::function_call_node* node,
		std::monostate context
	) const
	{
		std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> asts = {
			{ node->ast, node },
		};

		auto visit = [this](auto node) {
			return accept(node, *this, {});
		};

		std::ranges::copy(node->values | std::views::transform(visit) | std::views::join, inserter(asts, begin(asts)));
		std::ranges::copy(visit(node->consumer), inserter(asts, begin(asts)));

		return asts;
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::instantiation_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::conditional_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::block_instruction_node* node,
		std::monostate context
	) const
	{
		return accept(node->subject, *this, context);
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::array_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::expansion_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::local_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::unary_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::function_node* node,
		std::monostate context
	) const
	{
		std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> asts = {
			{ node->ast, node },
		};

		std::ranges::copy(accept(node->block, *this, {}), inserter(asts, begin(asts)));

		return asts;
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::global_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::return_node* node,
		std::monostate context
	) const
	{
		return accept(node->consumer, *this, context);
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::terminator_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::tuple_sink_node* node,
		std::monostate context
	) const
	{
		return {};
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::struct_node* node,
		std::monostate context
	) const
	{
		std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> asts = {
			{ node->ast, node },
		};

		return asts;
	}

	std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> visit(
		const fluenc::module_node& node,
		std::monostate context
	) const
	{
		std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> asts;

		auto visit = [this](auto pair) {
			return accept(pair.second, *this, {});
		};

		std::ranges::copy(node.functions | std::views::transform(visit) | std::views::join, inserter(asts, begin(asts)));
		std::ranges::copy(node.globals | std::views::transform(visit) | std::views::join, inserter(asts, begin(asts)));
		std::ranges::copy(node.types | std::views::transform(visit) | std::views::join, inserter(asts, begin(asts)));

		return asts;
	}
};
