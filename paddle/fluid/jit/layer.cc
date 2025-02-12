// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "paddle/fluid/jit/layer.h"

namespace paddle {
namespace jit {
// TODO(dev): Make vector<string>, num_slot as in argument
// Layer(const std::shared_ptr<ClassType>& type) : obj_(type, /*num_slot*/ 0U)
// {}
Layer::Layer(
    const std::vector<std::string>& func_names,
    const std::vector<framework::ProgramDesc>& program_descs,
    const std::vector<std::vector<std::string>>& param_names_for_each_program,
    const VariableNameMap& params_dict,
    const phi::Place& place) {
  VLOG(3) << "program size: " << program_descs.size();
  // Layer manage the life time of all parameter.
  for (size_t i = 0; i < func_names.size(); ++i) {
    // TODO(dev): choose exector or pe by flag
    function_dict[func_names[i]] = std::make_shared<ExectorFunction>(
        program_descs[i], param_names_for_each_program[i], params_dict, place);
  }
}

std::shared_ptr<BaseFunction> Layer::GetFunction(
    const std::string& name) const {
  VLOG(3) << "funcs_ size: " << function_dict.size();
  return function_dict.at(name);
}

std::vector<Variable> Layer::forward(const std::vector<Variable>& inputs) {
  auto func = GetFunction("forward");
  return (*func)(inputs);
}

}  // namespace jit
}  // namespace paddle
