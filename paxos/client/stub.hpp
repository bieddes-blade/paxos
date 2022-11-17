#pragma once

#include <consensus/value.hpp>

#include <whirl/rpc/use/channel.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>

namespace paxos {

using consensus::Value;

using await::fibers::Await;
using whirl::rpc::TChannel;

//////////////////////////////////////////////////////////////////////

class PaxosBlockingStub {
 public:
  explicit PaxosBlockingStub(TChannel& channel) : channel_(channel) {
  }

  Value Propose(Value value) {
    auto result = Await(channel_.Call("Consensus.Propose", value).As<Value>());
    return result.Value();
  }

 private:
  TChannel& channel_;
};

}  // namespace paxos
