#pragma once

#include <paxos/node/proposal.hpp>

#include <consensus/value.hpp>
#include <whirl/node/node_base.hpp>
#include <whirl/helpers/serialize.hpp>
#include <cereal/types/optional.hpp>

namespace paxos {

struct PrepInp {
  PropNum num;

  SERIALIZE(num)
};

struct PrepOut {
  PropNum np;
  PropNum na;
  std::optional<Value> value;
  bool check;

  SERIALIZE(np, na, value, check)
};

struct AccInp {
  PropNum num;
  std::optional<Value> value;

  SERIALIZE(num, value)
};

struct AccOut {
  PropNum num;
  PropNum np;
  bool check;

  SERIALIZE(num, np, check)
};

}  // namespace paxos
