#include <iostream>
#include <fstream>
#include <unordered_map>
#include <regex>

#include <jsonrpccxx/server.hpp>

#include <peglib.h>

#include <libfluenc-lang/Visitor.h>
#include <libfluenc-lang/ModuleInfo.h>
#include <libfluenc-lang/CallableNode.h>

#include "protocol/ClientCapabilities.h"
#include "protocol/InitializeResult.h"
#include "protocol/DocumentSymbol.h"
#include "protocol/TextDocumentIdentifier.h"
#include "protocol/WorkspaceSymbol.h"
#include "protocol/TextDocumentItem.h"

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
            std::ranges::subrange matches {
                std::sregex_iterator(begin(block), end(block), regex),
                std::sregex_iterator()
            };

            for (auto &match : matches)
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

lsp::InitializeResult initialize(const lsp::ClientCapabilities &capabilities, uint64_t process_id, const std::string &root_path, const std::string &root_uri, const std::vector<std::string> &workspace_folders)
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
            .documentHighlightProvider = true
        },
        .serverInfo = {
            .name = "fluenc-language-server",
            .version = "0.1"
        }
    };
}

struct foo
{
	foo()
//		: parser_(grammar)
	{
		parser_.log = [](size_t line, size_t col, const std::string &msg)
		{
			std::cerr << line << ":" << col << ": " << msg << "\n";
		};

		parser_.enable_ast();
		parser_.enable_packrat_parsing();
	}

	std::vector<lsp::DocumentSymbol> document_symbol(const lsp::TextDocumentIdentifier &text_document)
	{
		auto document = documents_.find(text_document.uri);

		if (document == end(documents_))
		{
			return {};
		}

		auto [_, module] = *document;

		std::vector<lsp::DocumentSymbol> symbols;

		for (auto [name, _] : module.functions)
		{
			std::cerr << name << std::endl;

			symbols.emplace_back(name, lsp::SymbolKind::Function);
		}

		return symbols;
	}

	std::vector<lsp::WorkspaceSymbol> workspace_symbol(const std::string &query)
	{
		std::vector<lsp::WorkspaceSymbol> symbols;

		for (auto &[uri, module] : documents_)
		{
			for (auto [name, node] : module.functions)
			{
				if (name.find(query) == std::string::npos)
				{
					continue;
				}

				auto ast = node->ast();

				lsp::WorkspaceSymbol symbol {
					.name = name,
					.kind = lsp::SymbolKind::Function,
					.location = {
						.uri = uri,
						.range = {
							.start = {
								.line = ast->line - 1,
								.character = ast->column
							},
							.end = {
								.line = ast->line - 1,
								.character = ast->column + ast->length
							}
						}
					}
				};

				symbols.push_back(symbol);
			}
		}

		return symbols;
	}

	void did_open(const lsp::TextDocumentItem &text_document)
	{
		Visitor visitor({}, nullptr, nullptr, nullptr, nullptr);

		std::shared_ptr<peg::Ast> ast;

		parser_.parse(text_document.text, ast, text_document.uri.c_str());

		documents_[text_document.uri] = visitor.visit(ast);
	}

private:
	peg::parser parser_;

	std::unordered_map<std::string, ModuleInfo> documents_;
};

struct server
{
	std::function<lsp::InitializeResult(const lsp::ClientCapabilities &capabilities, uint64_t process_id, const std::string &root_path, const std::string &root_uri, const std::vector<std::string> &workspace_folders)> initialize;

	struct
	{
		std::function<std::vector<lsp::DocumentSymbol>(const lsp::TextDocumentIdentifier &text_document)> documentSymbol;
		std::function<void(const lsp::TextDocumentItem &text_document)> didOpen;
	} textDocument;

	struct
	{
		std::function<std::vector<lsp::WorkspaceSymbol>(const std::string &query)> symbol;
	} workspace;
};

inline void to_json(nlohmann::json &target, std::optional<bool> source)
{
}

	static constexpr decltype(auto) grammar = R"(
Program             <- Instruction*
Instruction         <- Function / Structure / Global / Namespace / Use

# Instructions
Function            <- RegularFunction / ImportedFunction / ExportedFunction
Structure           <- 'struct' Id TypeList FieldList
Global              <- 'global' Id ':' Expression
Namespace           <- 'namespace' Id '{' Instruction* '}'
Use                 <- 'use' String

# Constructs
Block               <- '{' (Local / Call / Conditional)* Return '}'
Field               <- DecoratedField / StandardField
Return              <- 'return' Expression ('->' Continuation)?
Continuation        <- Id '(' List(Expression)? ')'
Argument            <- StandardArgument / TupleArgument
TypeName            <- FunctionType / RegularType
Expression          <- Unary / Binary / Instantiation / Literal / Tuple / Group / Tail / Expansion / Call / Member / Conditional / Array / Local
Comment             <- '//' [^\r\n]*
Assignment          <- Id ':' Expression
With                <- 'with' '{' List(Assignment) ','? '}'

# Lists
TypeList            <- (':' List(TypeName))?
FieldList           <- ('{' List(Field) ','? '}')?
IdList              <- Id ('.' Id)*

# Types
RegularType         <- Id
FunctionType        <- 'function' '(' List(RegularType)? ')'

# Fields
DecoratedField      <- TypeName Id (':' Expression)?
StandardField       <- Id (':' Expression)?

# Arguments
StandardArgument    <- TypeName Id
TupleArgument       <- '(' Argument (',' Argument)* ')'

# Functions
RegularFunction     <- 'function' Id '(' List(Argument)? ')' Block
ImportedFunction    <- 'import' TypeName Id '(' List(Argument)? ')'
ExportedFunction    <- 'export' TypeName Id '(' List(Argument)? ')' Block

# Expressions
Literal             <- ByteLiteral / Float32Literal / Uint32Literal / Int64Literal / Int32Literal / BooleanLiteral / StringLiteral / CharLiteral / NothingLiteral
Binary              <- (Group BinaryOperator Expression) / (Literal BinaryOperator Expression) / (Member BinaryOperator Expression) / (Call BinaryOperator Expression) / (Array BinaryOperator Expression)
Unary               <- (UnaryOperator Group) / (UnaryOperator Literal) / (UnaryOperator Member) / (UnaryOperator Call)
Member              <- IdList With?
Tail                <- 'tail' Id '(' List(Expression)? ')'
Call                <- Id '(' List(Expression)? ')'
Instantiation       <- TypeName '{' (List(Assignment) ','?)? '}'
Conditional         <- 'if' '(' Expression ')' Block
Array               <- '[' (List(Expression) ','?)? ']'
Group               <- '(' Expression ')'
Expansion           <- '...' Expression
Local               <- 'let' Id '=' Expression
Tuple               <- '(' Expression (',' Expression)+ ')'

# Literals
Int32Literal        <- Integer
Int64Literal        <- Integer 'i64'
Float32Literal      <- < Integer '.' Integer >
BooleanLiteral      <- < 'true' | 'false' >
StringLiteral       <- String
Uint32Literal       <- Integer 'u32'
CharLiteral         <- < '\'' < [^\']+ > '\'' >
ByteLiteral         <- Integer 'u8'
NothingLiteral      <- 'nothing'

# Primitives
BinaryOperator      <- '<=' | '>=' | '==' | '!=' | '&&' | '||' | '-' | '+' | '/' | '*' | '%' | '<' | '>' | '|' | '&' | '^' | '|'
UnaryOperator       <- '!' | '@'
Integer             <- < ('0x'[0-9A-F]+) / ('-'?[0-9]+) >
String              <- < '"' < [^"]* > '"' >
Keyword             <- 'return' | 'if' | 'let'
Id                  <- < (!Keyword '::'? Char (Char / Digit / '::')*) / '...' >
Char                <- [a-zA-Z_]
Digit               <- [0-9]
List(T)             <- T (',' T)*


%whitespace         <- (Comment / [ \t\r\n;])*
)";

int main()
{
	std::cerr << "fluenc-language-server started" << std::endl;

	try
	{
		jsonrpccxx::JsonRpc2Server server;

		peg::parser parser(grammar);
		parser.log = [](size_t line, size_t col, const std::string &msg)
		{
			std::cerr << line << ":" << col << ": " << msg << "\n";
		};

		parser.enable_ast();
		parser.enable_packrat_parsing();

		std::unordered_map<std::string, ModuleInfo> documents;

		::server s {
			.initialize = [](const lsp::ClientCapabilities &capabilities, uint64_t process_id, const std::string &root_path, const std::string &root_uri, const std::vector<std::string> &workspace_folders) -> lsp::InitializeResult {
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
						.documentHighlightProvider = true
					},
					.serverInfo = {
						.name = "fluenc-language-server",
						.version = "0.1"
					}
				};
			},
			.textDocument = {
				.documentSymbol = [&](const lsp::TextDocumentIdentifier &text_document) -> std::vector<lsp::DocumentSymbol> {
					auto document = documents.find(text_document.uri);

					if (document == end(documents))
					{
						return {};
					}

					auto [_, module] = *document;

					std::vector<lsp::DocumentSymbol> symbols;

					for (auto [name, _] : module.functions)
					{
						std::cerr << name << std::endl;

						symbols.emplace_back(name, lsp::SymbolKind::Function);
					}

					return symbols;
				},
				.didOpen = [&](const lsp::TextDocumentItem &text_document) {
					Visitor visitor({}, nullptr, nullptr, nullptr, nullptr);

					std::shared_ptr<peg::Ast> ast;

					parser.parse(text_document.text, ast, text_document.uri.c_str());

					documents[text_document.uri] = visitor.visit(ast);
				}
			},
			.workspace = {
				.symbol = [&](const std::string &query) -> std::vector<lsp::WorkspaceSymbol> {
					std::vector<lsp::WorkspaceSymbol> symbols;

					for (auto &[uri, module] : documents)
					{
						for (auto [name, node] : module.functions)
						{
							if (name.find(query) == std::string::npos)
							{
								continue;
							}

							auto ast = node->ast();

							lsp::WorkspaceSymbol symbol {
								.name = name,
								.kind = lsp::SymbolKind::Function,
								.location = {
									.uri = uri,
									.range = {
										.start = {
											.line = ast->line - 1,
											.character = ast->column
										},
										.end = {
											.line = ast->line - 1,
											.character = ast->column + ast->length
										}
									}
								}
							};

							symbols.push_back(symbol);
						}
					}

					return symbols;
				}
			}
		};

		if (s.initialize)
		{
			server.Add("initialize", jsonrpccxx::GetHandle(s.initialize), { "capabilities", "processId", "rootPath", "rootUri", "workspaceFolders" });
		}

		if (s.textDocument.documentSymbol)
		{
			server.Add("textDocument/documentSymbol", jsonrpccxx::GetHandle(s.textDocument.documentSymbol), { "textDocument" });
		}

		if (s.textDocument.didOpen)
		{
			server.Add("textDocument/didOpen", jsonrpccxx::GetHandle(s.textDocument.didOpen), { "textDocument" });
		}

		if (s.workspace.symbol)
		{
			server.Add("workspace/symbol", jsonrpccxx::GetHandle(s.workspace.symbol), { "query" });
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
	catch (const jsonrpccxx::JsonRpcException &e)
	{
		std::cerr << e.what() << std::endl;
	}

    return 0;
}
