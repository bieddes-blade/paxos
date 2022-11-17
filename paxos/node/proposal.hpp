#pragma once

#include <consensus/value.hpp>
#include <whirl/node/node_base.hpp>
#include <whirl/helpers/serialize.hpp>

namespace paxos {

////////////////////////////////////////////////////////////////////////////////

using Value = consensus::Value;

////////////////////////////////////////////////////////////////////////////////

// Proposal number

struct PropNum {
  int n;
  whirl::NodeId id;

  SERIALIZE(n, id)
};

bool operator<(const PropNum& x, const PropNum& y) {
  return std::tie(x.n, x.id) < std::tie(y.n, y.id);
}

bool operator>(const PropNum& x, const PropNum& y) {
  return std::tie(x.n, x.id) > std::tie(y.n, y.id);
}

bool operator>=(const PropNum& x, const PropNum& y) {
  return std::tie(x.n, x.id) >= std::tie(y.n, y.id);
}

bool operator==(const PropNum& x, const PropNum& y) {
  return std::tie(x.n, x.id) == std::tie(y.n, y.id);
}

////////////////////////////////////////////////////////////////////////////////

// Proposal

}  // namespace paxos
