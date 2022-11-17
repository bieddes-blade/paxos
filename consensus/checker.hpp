#pragma once

#include <consensus/value.hpp>

#include <whirl/matrix/history/history.hpp>

#include <set>

namespace consensus {

using whirl::histories::History;

class SafetyChecker {
 public:
  SafetyChecker(const History& history) {
    Load(history);
  }

  bool Safe() const {
    return Agreement() && Validity();
  }

  bool Agreement() const {
    return outputs_.size() == 1;
  }

  bool Validity() const {
    const Value output = *outputs_.begin();
    return inputs_.count(output) > 0;
  }

 private:
  void Load(const History& history) {
    for (const auto& call : history) {
      auto [input] = call.arguments.As<Value>();
      inputs_.insert(input);

      if (call.IsCompleted()) {
        auto output = call.result.As<Value>();
        outputs_.insert(output);
      }
    }
  }

 private:
  std::set<Value> inputs_;
  std::set<Value> outputs_;
};

bool IsSafe(const History& history) {
  SafetyChecker checker(history);
  return checker.Safe();
}

}   // namespace consensus
