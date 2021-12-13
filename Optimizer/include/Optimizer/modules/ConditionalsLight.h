/**
* \file OptimizeConditionalsLight.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for OptimizeConditionalsLight optimzer module
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
#ifndef RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_CONDTION_IS_KNOWN_H
#define RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_CONDTION_IS_KNOWN_H

#include "shared/AST/AST.h"
#include "shared/AST/NodeData.h"
#include "shared/AST/NodeHasValue.h"
#include "OptimizerModule.h"

#include <iterator>

#define RAYCHELSCRIPT_WRAP_CALL(expression)                                                                                      \
    if ((expression)) {                                                                                                          \
        should_increment = true;                                                                                                 \
        continue;                                                                                                                \
    }

namespace RaychelScript::Optimizer {

    struct OptimizeConditionalsLight final : public OptimizerModule
    {
        OptimizeConditionalsLight() = default;
        RAYCHEL_MAKE_NONCOPY_NONMOVE(OptimizeConditionalsLight)

        [[nodiscard]] std::string_view name() const noexcept override
        {
            return "Light-weight optimizations on conditional constructs";
        }

        void operator()(AST& ast) const noexcept override
        {
            for (auto it = ast.nodes.begin(); it != ast.nodes.end();) {
                bool should_increment = true;
                if (it->type() != NodeType::conditional_construct) {
                    it++;
                    continue;
                }

                auto data = it->to_node_data<ConditionalConstructData>();

                RAYCHELSCRIPT_WRAP_CALL(_remove_if_condition_is_literal_false(data, it, ast.nodes))
                RAYCHELSCRIPT_WRAP_CALL(_remove_if_condition_is_literal_true(data, it, ast.nodes))
                RAYCHELSCRIPT_WRAP_CALL(_remove_if_condition_body_is_empty(data, it, ast.nodes))

                if (should_increment) {
                    it++;
                }
            }
        }

        ~OptimizeConditionalsLight() override = default;

    private:
        [[nodiscard]] bool
        _remove_if_condition_is_literal_false(const ConditionalConstructData& data, auto& it, auto& nodes) const noexcept
        {
            if (!node_has_known_value(data.condition_node)) {
                return false;
            }

            if (data.condition_node.type() != NodeType::literal_false) {
                return false;
            }

            it = nodes.erase(it); //remove 'if false'
            return true;
        }

        [[nodiscard]] bool
        _remove_if_condition_is_literal_true(ConditionalConstructData& data, auto& it, auto& nodes) const noexcept
        {
            if (!node_has_known_value(data.condition_node)) {
                return false;
            }

            if (data.condition_node.type() != NodeType::literal_true) {
                return false;
            }

            //save where we left our condition node (we can't use iterators because they are invalidated by insert())
            const auto condition_index = std::distance(nodes.begin(), it);

            //Because conditionals push the state, we need to do the same
            it = nodes.insert(std::next(it), AST_Node{InlinePushData{{}}});

            //insert all body nodes behind our condition node
            for (auto& node : data.body) {
                it = nodes.insert(std::next(it), std::move(node));
            }

            //Because conditionals pop the state, we need to do the same
            it = nodes.insert(std::next(it), AST_Node{InlinePopData{{}}});

            //remove the condition node
            const auto conditon_iterator = nodes.begin() + condition_index;
            it = nodes.erase(conditon_iterator);

            return true;
        }

        [[nodiscard]] bool
        _remove_if_condition_body_is_empty(const ConditionalConstructData& data, auto& it, auto& nodes) const noexcept
        {
            if (!data.body.empty()) {
                return false;
            }

            it = nodes.erase(it);
            return true;
        }
    };

} //namespace RaychelScript::Optimizer

#undef RAYCHELSCRIPT_WRAP_CALL

#endif //!RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_CONDTION_IS_KNOWN_H