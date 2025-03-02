#pragma once

#include <peglib.h>

#include <fluenc-lang/ast.hpp>
#include <fluenc-lang/ast/module_node.hpp>
#include <fluenc-lang/comfy_stack.hpp>
#include <fluenc-lang/expression.hpp>

#include <unordered_map>

#include "protocol/Location.h"

struct default_type;
struct user_type;

using default_type_t = std::unique_ptr<default_type>;
using user_type_t = std::unique_ptr<user_type>;

// using type_t = std::variant<default_type_t, user_type_t>;

struct default_type
{
	type_name_t name;
};

struct user_type
{
	// std::unordered_map<std::string, type_t> fields;
};

default_type_t from_type_name(const type_name_t& name)
{
	return std::make_unique<default_type>(name);
}

// using type_stack_t = fluenc::comfy_stack<type_t>;

struct type_resolver_state
{
	// std::unordered_map<std::string, type_t> locals;
};

struct type_resolver_context
{
	type_resolver_state state;
	// type_stack_t types;
};

bool type_names_equal(const type_name_t& left, const type_name_t& right);

bool compare_type_names(auto& left, auto& right)
{
	return false;
}

bool compare_type_names(const default_type_name& left, const default_type_name& right)
{
	return std::ranges::any_of(left.names, [&](auto name) {
		return std::ranges::find(right.names, name) != end(right.names);
	});
}

bool compare_type_names(const function_type_name& left, const function_type_name& right)
{
	if (size(left.types) != size(right.types))
	{
		return false;
	}

	auto zipped = std::views::zip(left.types, right.types);

	return std::ranges::all_of(zipped, [](auto pair) {
		return std::apply(type_names_equal, pair);
	});
}

bool type_names_equal(const type_name_t& left, const type_name_t& right)
{
	return std::visit(
		[](auto left, auto right) -> bool {
			return compare_type_names(left, right);
		},
		left,
		right
	);
}

bool types_equal_impl(const auto& left, const auto& right)
{
	return false;
}

// bool types_equal_impl(const default_type_t& left, const default_type_t& right)
// {
// 	return type_names_equal(left->name, right->name);
// }

// bool types_equal_impl(const user_type_t& left, const user_type_t& right)
// {
// 	return false;
// }

// bool types_equal(const type_t& left, const type_t& right)
// {
// 	return std::visit(
// 		[](const auto& left, const auto& right) -> bool {
// 			return false;
// 			// return types_equal_impl(left, right);
// 		},
// 		left,
// 		right
// 	);
// }

// struct type_resolver
// {
// 	using return_type = std::vector<type_t>;
// 	using context_type = type_resolver_context;

// 	return_type visit(const fluenc::binary_node* node, context_type context) const
// 	{
// 		// auto left = context.types.pop();
// 		// auto right = context.types.pop();

// 		// if (!types_equal(left, right))
// 		// {
// 		// 	throw std::exception();
// 		// }

// 		// context.types.push(std::move(left));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::integral_literal_node* node, context_type context) const
// 	{
// 		// context.types.push(from_type_name(node->type));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::float_literal_node* node, context_type context) const
// 	{
// 		// context.types.push(from_type_name(node->type));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::boolean_literal_node* node, context_type context) const
// 	{
// 		// context.types.push(from_type_name(default_type_name::boolean()));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::string_literal_node* node, context_type context) const
// 	{
// 		// context.types.push(from_type_name(default_type_name::string()));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::character_literal_node* node, context_type context) const
// 	{
// 		// context.types.push(from_type_name(default_type_name::byte()));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::nothing_node* node, context_type context) const
// 	{
// 		// context.types.push(from_type_name(default_type_name::without()));

// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::member_access_node* node, context_type context) const
// 	{
// 		for (auto& name : node->names)
// 		{
// 			if (auto locals_it = context.state.locals.find(name); locals_it != end(context.state.locals))
// 			{
// 				// context.types.push(std::move(locals_it->second));

// 				return accept(node->consumer, *this, context);
// 			}
// 		}

// 		throw std::exception();
// 	}

// 	return_type visit(const fluenc::function_call_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::instantiation_node* node, context_type context) const
// 	{
// 		// for (auto i = 0; i < size(node->fields); i++)
// 		// {
// 		// 	context.types.pop();
// 		// }

// 		// std::visit(
// 		// 	[](auto prototype) {
// 		// 		using type = decltype(prototype);

// 		// 		if constexpr (std::is_same_v<type, fluenc::default_prototype>)
// 		// 		{
// 		// 		}
// 		// 	},
// 		// 	node->prototype
// 		// );

// 		// context.types.push(node->prototype);

// 		return {};
// 	}

// 	return_type visit(const fluenc::conditional_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::block_instruction_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::array_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::expansion_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::local_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::unary_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::function_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::global_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::return_node* node, context_type context) const
// 	{
// 		return accept(node->consumer, *this, context);
// 	}

// 	return_type visit(const fluenc::terminator_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::tuple_sink_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::struct_node* node, context_type context) const
// 	{
// 		return {};
// 	}

// 	return_type visit(const fluenc::module_node& node, context_type context) const
// 	{
// 		for (auto& [_, function] : node.functions)
// 		{
// 			if (auto types = accept(function, *this, context); !empty(types))
// 			{
// 				return types;
// 			}
// 		}

// 		for (auto& [_, global] : node.globals)
// 		{
// 			if (auto types = accept(global, *this, context); !empty(types))
// 			{
// 				return types;
// 			}
// 		}

// 		for (auto& [_, type] : node.types)
// 		{
// 			if (auto types = accept(type, *this, context); !empty(types))
// 			{
// 				return types;
// 			}
// 		}

// 		return {};
// 	}

// private:
// 	std::unordered_map<std::string, fluenc::module_node> documents_;
// };
