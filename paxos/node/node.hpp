#pragma once

// Paxos roles
#include <paxos/node/proposer.hpp>
#include <paxos/node/acceptor.hpp>
#include <paxos/node/learner.hpp>

#include <whirl/node/node_base.hpp>

namespace paxos {

using whirl::NodeBase;
using whirl::rpc::IServerPtr;

////////////////////////////////////////////////////////////////////////////////

// Paxos node = Proposer + Acceptor [+ Learner]

class PaxosNode : public NodeBase {
 public:
  PaxosNode(NodeServices services) : NodeBase(services) {
  }

 protected:
  void RegisterRPCServices(const IServerPtr& rpc_server) override {
    // Proposer role
    rpc_server->RegisterService("Consensus",
                                std::make_shared<Proposer>(ThisNodeServices()));

    // Acceptor role
    rpc_server->RegisterService("Acceptor",
                                std::make_shared<Acceptor>(ThisNodeServices()));

    // Learner?
  }

  void MainThread() override {
    // Do nothing
  }
};

}  // namespace paxos
