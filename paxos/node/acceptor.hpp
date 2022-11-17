#pragma once

#include <paxos/node/proposal.hpp>
#include <paxos/node/protocol.hpp>

#include <whirl/node/logging.hpp>
#include <whirl/node/peer_base.hpp>
#include <whirl/rpc/use/service_base.hpp>

#include <future>
#include <cereal/types/optional.hpp>

namespace paxos {

using whirl::NodeServices;
using whirl::PeerBase;
using whirl::rpc::ServiceBase;

////////////////////////////////////////////////////////////////////////////////

// Acceptor role / RPC service

class Acceptor : public ServiceBase<Acceptor>, public PeerBase {
 public:
  Acceptor(NodeServices services) : PeerBase(std::move(services)) {
  }

 protected:
  PropNum np;
  PropNum na;
  std::optional<Value> va = {};

  void RegisterRPCMethods() {
    RPC_REGISTER_METHOD(Prepare);
    RPC_REGISTER_METHOD(Accept);
  }

  PrepOut Prepare(PrepInp input) {
    PropNum n = input.num;
    PrepOut ans;

    if (n > np) {
      np = n;

      // reply prepare_ok(n, na, va);
      ans.np = np;
      ans.na = na;
      ans.value = va;
      ans.check = true;
    } else {
      // reply prepare_reject;
      ans.check = false;
    }

    return ans;
  }

  AccOut Accept(AccInp input) {
    PropNum n = input.num;
    std::optional<Value> v = input.value;
    AccOut ans;

    if (n >= np) {
      np = n;
      na = n;
      va = v;

      // reply accept_ok(n);
      ans.num = np;
      ans.np = np;
      ans.check = true;

    } else {
      // reply accept_reject;
      ans.check = false;
    }

    return ans;
  }
};

}  // namespace paxos
