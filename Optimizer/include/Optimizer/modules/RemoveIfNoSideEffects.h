/**
* \file RemoveIfNoSideEffects.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for RemoveIfNoSideEffects optimizer module
* \date 2021-12-04
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
#ifndef RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_NO_SIDEEFFECTS_H
#define RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_NO_SIDEEFFECTS_H

#include "OptimizerModule.h"

namespace RaychelScript::Optimizer {

    class RemoveIfNoSideEffects final : public OptimizerModule
    {
    public:
        RemoveIfNoSideEffects() = default;

        RAYCHEL_MAKE_NONCOPY_NONMOVE(RemoveIfNoSideEffects)

        [[nodiscard]] std::string_view name() const noexcept override
        {
            using namespace std::string_view_literals;
            return "Remove nodes with no side effects"sv;
        }

        void operator()(AST& ast) const noexcept override
        {
            _remove_children_if_no_side_effect(ast.nodes);
        }

        ~RemoveIfNoSideEffects() override = default;

    private:
        void _remove_children_if_no_side_effect(std::vector<AST_Node>& nodes) const noexcept
        {
            for (auto it = nodes.begin(); it != nodes.end();) {
                if (it->type() == NodeType::conditional_construct) {
                    _handle_conditional_children(it);
                    it++;
                } else if (!it->has_side_effect()) {
                    it = nodes.erase(it);
                } else {
                    it++;
                }
            }
        }

        void _handle_conditional_children(std::vector<AST_Node>::iterator it) const noexcept
        {
            auto data = it->to_node_data<ConditionalConstructData>();
            _remove_children_if_no_side_effect(data.body);

            *it = AST_Node{std::move(data)};
        }
    };

} //namespace RaychelScript::Optimizer

#endif //!RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_NO_SIDEEFFECTS_H