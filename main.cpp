#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <unordered_map>

#include <jsonrpccxx/client.hpp>
#include <jsonrpccxx/server.hpp>

#include <peglib.h>

#include <fluenc-lang/ast.hpp>
#include <fluenc-lang/ast/module_node.hpp>
#include <fluenc-lang/ast_transformer.hpp>
#include <fluenc-lang/expression.hpp>

#include <fluenc-grammar/grammar.h>

#include "protocol/ClientCapabilities.h"
#include "protocol/Diagnostic.h"
#include "protocol/DocumentSymbol.h"
#include "protocol/Hover.h"
#include "protocol/InitializeResult.h"
#include "protocol/TextDocumentContentChangeEvent.h"
#include "protocol/TextDocumentIdentifier.h"
#include "protocol/TextDocumentItem.h"
#include "protocol/VersionedTextDocumentIdentifier.h"
#include "protocol/WorkspaceFolder.h"
#include "protocol/WorkspaceSymbol.h"

#include <SDL2/SDL.h>

#include "ast_extractor.hpp"
#include "location_resolver.hpp"
#include "type_resolver.hpp"

INCBIN_EXTERN(Grammar);

template <typename T>
concept has_ast = requires(T t) {
	{
		t.ast
	} -> std::same_as<std::shared_ptr<peg::Ast>>;
};

using headers_t = std::unordered_map<std::string, std::string>;

headers_t read_header()
{
	std::cerr << "Reading header..." << std::endl;

	using traits_t = decltype(std::cin)::traits_type;

	std::string block;
	std::regex regex("([^:\\s]+)\\s*:\\s*([^\\s]+)");

	headers_t headers;

	for (auto c = 0; (c = std::cin.get()) != traits_t::eof();)
	{
		block += c;

		std::cout << c << std::endl;

		if (block.ends_with("\r\n\r\n"))
		{
			std::ranges::subrange matches { std::sregex_iterator(begin(block), end(block), regex),
											std::sregex_iterator() };

			for (auto& match : matches)
			{
				if (size(match) < 2)
				{
					continue;
				}

				headers.insert({ match[1].str(), match[2].str() });
			}

			return headers;
		}
	}

	return headers;
}

struct stdout_connector : public jsonrpccxx::IClientConnector
{
	std::string Send(const std::string& request) override
	{
		std::cout << "Content-Length: " << size(request) << "\r\n\r\n";
		std::cout << request << std::endl;

		auto headers = read_header();

		auto content_length_it = headers.find("Content-Length");

		if (content_length_it == end(headers))
		{
			return {};
		}

		auto content_length = std::stoi(content_length_it->second);

		std::string body;
		body.resize(content_length);

		std::cin.read(body.data(), content_length);

		return body;
	}
};

stdout_connector connector;

jsonrpccxx::JsonRpcClient client(connector, jsonrpccxx::version::v2);

struct server
{
	std::function<lsp::InitializeResult(
		const lsp::ClientCapabilities& capabilities,
		uint64_t process_id,
		const std::string& root_path,
		const std::string& root_uri,
		const std::vector<lsp::WorkspaceFolder>& workspace_folders
	)>
		initialize;

	struct
	{
		std::function<std::vector<lsp::DocumentSymbol>(const lsp::TextDocumentIdentifier& text_document)> documentSymbol;
		std::function<void(const lsp::TextDocumentItem& text_document)> didOpen;
		std::function<void(
			const lsp::VersionedTextDocumentIdentifier& textDocument,
			const std::vector<lsp::TextDocumentContentChangeEvent>& contentChanges
		)>
			didChange;
		std::function<void(const lsp::TextDocumentIdentifier& textDocument, const lsp::Position& position)> completion;
		std::function<lsp::Hover(const lsp::TextDocumentIdentifier& textDocument, const lsp::Position& position)> hover;
		std::function<std::optional<
			std::vector<lsp::Location>>(const lsp::TextDocumentIdentifier& textDocument, const lsp::Position& position)>
			definition;
	} textDocument;

	struct
	{
		std::function<std::vector<lsp::WorkspaceSymbol>(const std::string& query)> symbol;
	} workspace;
};

inline void to_json(nlohmann::json& target, std::optional<bool> source)
{
}

std::unordered_map<std::string, fluenc::module_node> documents;
std::unordered_map<std::shared_ptr<peg::Ast>, fluenc::expression_t> asts;

lsp::InitializeResult initialize(
	const lsp::ClientCapabilities& capabilities,
	uint64_t process_id,
	const std::string& root_path,
	const std::string& root_uri,
	const std::vector<lsp::WorkspaceFolder>& workspace_folders
)
{
	return {
		.capabilities = {
			// .position_encoding = lsp::position_encoding_kind::utf8,
			.textDocumentSync = lsp::TextDocumentSyncOptions {
				.openClose = true,
				.change = lsp::TextDocumentSyncKind::Full
			},
			.completionProvider = lsp::CompletionOptions {
				.resolveProvider = true,
			},
			.documentHighlightProvider = true,
			.documentSymbolProvider = true,
			.referencesProvider = true,
			.declarationProvider = true,
		},
		.serverInfo = {
			.name = "fluenc-language-server",
			.version = "0.1"
		},
	};
}

std::vector<lsp::DocumentSymbol> document_symbol(const lsp::TextDocumentIdentifier& text_document)
{
	auto document = documents.find(text_document.uri);

	if (document == end(documents))
	{
		return {};
	}

	auto [_, module] = *document;

	std::vector<lsp::DocumentSymbol> symbols;

	for (auto [name, function] : module.functions)
	{
		std::cerr << name << std::endl;

		auto& ast = function->ast;

		if (!ast)
		{
			continue;
		}

		lsp::Range range = {
			.start = { 
				.line = ast->line - 1, 
				.character = ast->column - 1,
			},
			.end = { 
				.line = ast->line - 1, 
				.character = ast->column + ast->length - 1, 
			},
		};

		lsp::DocumentSymbol symbol {
			.name = name,
			.kind = lsp::SymbolKind::Function,
			.range = range,
			.selectionRange = range,
		};

		symbols.push_back(symbol);
	}

	return symbols;
}

void did_open(const lsp::TextDocumentItem& text_document)
{
	fluenc::ast_transformer visitor({}, {}, {}, {}, {});

	std::shared_ptr<peg::Ast> ast;

	peg::parser parser(grammar());
	parser.set_logger([&](size_t line, size_t col, const std::string& msg) {
		std::vector<lsp::Diagnostic> diagnostic = {
			{
				.range = {
					.start = {
						.line = line - 1,
						.character = col - 1,
					},
					.end = {
						.line = line - 1,
						.character = col - 1,
					}
				},
				.severity = lsp::DiagnosticSeverity::Error,
				.message = msg
			}
		};

		client.CallNotificationNamed(
			"textDocument/publishDiagnostics",
			{
				{ "uri", text_document.uri },
				{ "diagnostics", diagnostic },
			}
		);

		std::cerr << line << ":" << col << ": " << msg << "\n";
	});

	parser.enable_ast();
	parser.enable_packrat_parsing();

	if (!parser.parse(text_document.text, ast, text_document.uri.c_str()))
	{
		return;
	}

	client.CallNotificationNamed(
		"textDocument/publishDiagnostics",
		{
			{ "uri", text_document.uri },
			{ "diagnostics", {} },
		}
	);

	ast_extractor extractor;

	auto k = visitor.transform(ast);
	auto lasts = fluenc::accept(k, extractor, {});

	std::ranges::copy(lasts, inserter(asts, begin(asts)));

	documents[text_document.uri] = k;
}

void did_change(
	const lsp::VersionedTextDocumentIdentifier& text_document,
	const std::vector<lsp::TextDocumentContentChangeEvent>& content_changes
)
{
	if (empty(content_changes))
	{
		return;
	}

	fluenc::ast_transformer visitor({}, {}, {}, {}, {});

	std::shared_ptr<peg::Ast> ast;

	peg::parser parser(grammar());
	parser.set_logger([&](size_t line, size_t col, const std::string& msg) {
		std::vector<lsp::Diagnostic> diagnostic = {
			{
				.range = {
					.start = {
						.line = line - 1,
						.character = col - 1,
					},
					.end = {
						.line = line - 1,
						.character = col - 1,
					}
				},
				.severity = lsp::DiagnosticSeverity::Error,
				.message = msg
			}
		};

		client.CallNotificationNamed(
			"textDocument/publishDiagnostics",
			{
				{ "uri", text_document.uri },
				{ "diagnostics", diagnostic },
			}
		);

		std::cerr << line << ":" << col << ": " << msg << "\n";
	});

	parser.enable_ast();
	parser.enable_packrat_parsing();

	if (!parser.parse(content_changes[0].text, ast, text_document.uri.c_str()))
	{
		return;
	}

	client.CallNotificationNamed(
		"textDocument/publishDiagnostics",
		{
			{ "uri", text_document.uri },
			{ "diagnostics", {} },
		}
	);

	ast_extractor extractor;

	auto expressions = visitor.transform(ast);
	auto lasts = fluenc::accept(expressions, extractor, {});

	std::ranges::copy(lasts, inserter(asts, begin(asts)));

	documents[text_document.uri] = expressions;
}

std::vector<lsp::WorkspaceSymbol> symbol(const std::string& query)
{
	std::vector<lsp::WorkspaceSymbol> symbols;

	for (auto& [uri, module] : documents)
	{
		for (auto [name, node] : module.functions)
		{
			if (name.find(query) == std::string::npos)
			{
				continue;
			}

			auto ast = node->ast;

			if (!ast)
			{
				continue;
			}

			lsp::WorkspaceSymbol symbol {
				.name = name,
				.kind = lsp::SymbolKind::Function,
				.location = {
					.uri = uri,
					.range = {
						.start = {
							.line = ast->line - 1,
							.character = ast->column - 1
						},
						.end = {
							.line = ast->line - 1,
							.character = ast->column + ast->length - 1,
						},
					},
				},
			};

			symbols.push_back(symbol);
		}

		for (auto [name, type] : module.types)
		{
			if (name.find(query) == std::string::npos)
			{
				continue;
			}

			auto ast = type->ast;

			if (!ast)
			{
				continue;
			}

			lsp::WorkspaceSymbol symbol {
				.name = name,
				.kind = lsp::SymbolKind::Struct,
				.location = {
					.uri = uri,
					.range = {
						.start = {
							.line = ast->line - 1,
							.character = ast->column - 1
						},
						.end = {
							.line = ast->line - 1,
							.character = ast->column + ast->length - 1,
						},
					},
				},
			};

			symbols.push_back(symbol);
		}

		for (auto [name, global] : module.globals)
		{
			if (name.find(query) == std::string::npos)
			{
				continue;
			}

			std::visit(
				[&](auto&& expression) {
					if constexpr (has_ast<decltype(expression)>)
					{
						auto ast = expression->ast;

						if (!ast)
						{
							return;
						}

						lsp::WorkspaceSymbol symbol {
							.name = name,
							.kind = lsp::SymbolKind::Variable,
							.location = {
								.uri = uri,
								.range = {
									.start = {
										.line = ast->line - 1,
										.character = ast->column - 1
									},
									.end = {
										.line = ast->line - 1,
										.character = ast->column + ast->length - 1,
									},
								},
							},
						};

						symbols.push_back(symbol);
					}
				},
				global
			);
		}
	}

	return symbols;
}

lsp::Hover hover(const lsp::TextDocumentIdentifier& text_document, const lsp::Position& position)
{
	return lsp::Hover { 
		.contents = {
			.kind = lsp::MarkupKind::PlainText,
			.value = "test",
		}, 
	};
}

std::optional<fluenc::expression_t> expression_at(const lsp::Position& position)
{
	auto it = std::ranges::find_if(asts, [position](auto item) -> bool {
		auto& [ast, expression] = item;

		if (ast->line != position.line + 1)
		{
			return false;
		}

		if (ast->column > position.character + 1)
		{
			return false;
		}

		if (ast->column + ast->length < position.character + 1)
		{
			return false;
		}

		std::cerr << "we found an ast at " << ast->line << ":" << ast->column << std::endl;

		return true;
	});

	if (it != end(asts))
	{
		return it->second;
	}

	return {};
}

std::optional<std::vector<lsp::Location>> definition(
	const lsp::TextDocumentIdentifier& text_document,
	const lsp::Position& position
)
{
	std::cerr << "number of asts: " << size(asts) << std::endl;

	if (auto expression = expression_at(position))
	{
		location_resolver resolver(documents);

		if (auto location = accept(*expression, resolver, {}))
		{
			return std::vector<lsp::Location> { *location };
		}
	}

	return {};
}

int main()
{
	std::ofstream out("/home/znurre/lsp.txt");
	std::cerr.rdbuf(out.rdbuf()); // redirect std::cout to out.txt!

	std::cerr << "fluenc-language-server started !!!" << std::endl;

	try
	{
		jsonrpccxx::JsonRpc2Server server;

		::server lsp {
			.initialize = initialize,
			.textDocument = {
				.documentSymbol = document_symbol,
				.didOpen = did_open,
				.didChange = did_change,
				.hover = hover,
				.definition = definition,
			},
			.workspace = {
				.symbol = symbol,
			},
		};

		if (lsp.initialize)
		{
			server.Add(
				"initialize",
				jsonrpccxx::GetHandle(lsp.initialize),
				{ "capabilities", "processId", "rootPath", "rootUri", "workspaceFolders" }
			);
		}

		if (lsp.textDocument.documentSymbol)
		{
			server
				.Add("textDocument/documentSymbol", jsonrpccxx::GetHandle(lsp.textDocument.documentSymbol), { "textDocument" });
		}

		if (lsp.textDocument.didOpen)
		{
			server.Add("textDocument/didOpen", jsonrpccxx::GetHandle(lsp.textDocument.didOpen), { "textDocument" });
		}

		if (lsp.textDocument.didChange)
		{
			server.Add(
				"textDocument/didChange",
				jsonrpccxx::GetHandle(lsp.textDocument.didChange),
				{ "textDocument", "contentChanges" }
			);
		}

		if (lsp.textDocument.completion)
		{
			server.Add(
				"textDocument/completion",
				jsonrpccxx::GetHandle(lsp.textDocument.completion),
				{ "textDocument", "position" }
			);
		}

		if (lsp.textDocument.hover)
		{
			server.Add("textDocument/hover", jsonrpccxx::GetHandle(lsp.textDocument.hover), { "textDocument", "position" });
		}

		if (lsp.textDocument.definition)
		{
			server.Add(
				"textDocument/definition",
				jsonrpccxx::GetHandle(lsp.textDocument.definition),
				{ "textDocument", "position" }
			);
		}

		if (lsp.workspace.symbol)
		{
			server.Add("workspace/symbol", jsonrpccxx::GetHandle(lsp.workspace.symbol), { "query" });
		}

		while (true)
		{
			auto headers = read_header();

			auto content_length_it = headers.find("Content-Length");

			if (content_length_it == end(headers))
			{
				return 0;
			}

			auto content_length = std::stoi(content_length_it->second);

			std::cerr << content_length << std::endl;

			std::string body;
			body.resize(content_length);

			std::cin.read(body.data(), content_length);

			std::cerr << body << std::endl;

			auto response = server.HandleRequest(body);

			std::cout << "Content-Length: " << response.size() << "\r\n\r\n";
			std::cout << response << std::endl;

			std::cerr << "Content-Length: " << response.size() << "\r\n\r\n";
			std::cerr << response << std::endl;
		}
	}
	catch (const jsonrpccxx::JsonRpcException& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
