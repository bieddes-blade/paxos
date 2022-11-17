#pragma once

#include <paxos/node/proposal.hpp>
#include <paxos/node/protocol.hpp>

#include <whirl/node/logging.hpp>
#include <whirl/node/peer_base.hpp>
#include <whirl/rpc/use/service_base.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/futures/combine/quorum.hpp>
#include <cereal/types/optional.hpp>

namespace paxos {

using whirl::NodeServices;
using whirl::PeerBase;
using whirl::rpc::ServiceBase;

using await::fibers::Await;
using await::futures::Future;
using await::futures::Quorum;

////////////////////////////////////////////////////////////////////////////////

// Proposer role / RPC service

class Proposer : public ServiceBase<Proposer>, public PeerBase {
 public:
  Proposer(NodeServices services) : PeerBase(std::move(services)) {
  }

 protected:
  int n = 0;

  void RegisterRPCMethods() {
    RPC_REGISTER_METHOD(Propose);
  }

  size_t Majority() const {
    return PeerCount() / 2 + 1;
  }

  Value Propose(Value input) {
    while (true) {
      n += 1;

      bool wrong = false;

      PropNum pn;
      pn.id = Id();
      pn.n = n;

      PrepInp pr_inp;
      pr_inp.num = pn;

      // send prepare messages

      std::vector<Future<PrepOut>> prepare_promises;
      for (size_t i = 0; i < PeerCount(); ++i) {
        prepare_promises.push_back(
            PeerChannel(i).Call("Acceptor.Prepare", pr_inp));
      }

      auto prepare_values =
          Await(Quorum(std::move(prepare_promises), Majority())).Value();

      // find highest na

      PropNum high_pn;
      std::optional<Value> high_value;
      bool has_value = false;

      for (size_t i = 0; i < prepare_values.size(); ++i) {
        if (!prepare_values[i].check) {
          wrong = true;
          break;
        }

        if (prepare_values[i].value &&
            (!has_value || prepare_values[i].na > high_pn)) {
          high_pn = prepare_values[i].na;
          high_value = prepare_values[i].value;
          has_value = true;
        }
      }

      if (wrong) {
        continue;
      }

      if (!high_value) {
        high_value = input;
      }

      AccInp ac_inp;
      ac_inp.num = pn;
      ac_inp.value = high_value;

      // send accept messages

      std::vector<Future<AccOut>> accept_promises;
      for (size_t i = 0; i < PeerCount(); ++i) {
        accept_promises.push_back(
            PeerChannel(i).Call("Acceptor.Accept", ac_inp));
      }

      auto accept_values =
          Await(Quorum(std::move(accept_promises), Majority())).Value();

      // check the values

      for (size_t i = 0; i < accept_values.size(); ++i) {
        if (!accept_values[i].check) {
          wrong = true;
          break;
        }
      }

      if (wrong) {
        continue;
      }

      return high_value.value();
    }
  }
};

}  // namespace paxos
