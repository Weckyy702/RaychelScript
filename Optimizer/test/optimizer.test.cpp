/**
* \file optimizer.test.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file Optimizer testing executable
* \date 2021-12-05
* 
* MIT License
* Copyright (c) [2021] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
*/

#include "Misc/PrintAST.h"
#include "OptimizerPipe.h"

/**
* \brief Very dumb algorithm to estimate the complexity of an AST
* 
* \param ast 
* \return std::size_t 
*/
static std::size_t estimate_ast_complexity(const RaychelScript::AST& ast) noexcept
{
    std::size_t complexity_score{0};

    //Currently, we count the number of children to make up an estimate for the complexity of the AST

    RaychelScript::for_each_node(ast, [&complexity_score](const RaychelScript::AST_Node& node) {
        switch (node.type()) {
            case RaychelScript::NodeType::assignment:
            case RaychelScript::NodeType::arithmetic_operator:
            case RaychelScript::NodeType::relational_operator:
                complexity_score += 3;
                break;
            case RaychelScript::NodeType::variable_decl:
            case RaychelScript::NodeType::variable_ref:
            case RaychelScript::NodeType::numeric_constant:
            case RaychelScript::NodeType::literal_true:
            case RaychelScript::NodeType::literal_false:
                complexity_score++;
                break;

            case RaychelScript::NodeType::unary_operator:
            case RaychelScript::NodeType::conditional_construct:
                complexity_score += 2;
                break;
        }
    });

    return complexity_score;
}

int main()
{
    using namespace RaychelScript::Pipes; //NOLINT(google-build-using-namespace)

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    const auto ast_or_error = Lex{lex_file, "../../../shared/test/optimizable.rsc"} | Parse{};
    const auto optimized_ast_or_error = ast_or_error | Optimize{RaychelScript::Optimizer::OptimizationLevel::all};

    if (const auto* ec = std::get_if<RaychelScript::Parser::ParserErrorCode>(&ast_or_error); ec) {
        Logger::log("Error during parsing: ", RaychelScript::Parser::error_code_to_reason_string(*ec), '\n');
        return 1;
    }

    if (const auto* ec = std::get_if<RaychelScript::Parser::ParserErrorCode>(&optimized_ast_or_error); ec) {
        Logger::log("Error during parsing: ", RaychelScript::Parser::error_code_to_reason_string(*ec), '\n');
        return 1;
    }

    const auto ast = Raychel::get<RaychelScript::AST>(ast_or_error);
    const auto optimized_ast = Raychel::get<RaychelScript::AST>(optimized_ast_or_error);

    Logger::info("Unoptimzed (complexity=", estimate_ast_complexity(ast), "):\n");
    RaychelScript::pretty_print_ast(ast);

    Logger::info("Optimized (complexity=", estimate_ast_complexity(optimized_ast), "):\n");
    RaychelScript::pretty_print_ast(optimized_ast);
}