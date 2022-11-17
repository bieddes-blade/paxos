#include <consensus/value.hpp>
#include <consensus/checker.hpp>
#include <consensus/printer.hpp>

#include <paxos/client/stub.hpp>
#include <paxos/node/node.hpp>

// Simulation

#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/client/client.hpp>
#include <whirl/matrix/history/printers/print.hpp>
#include <whirl/matrix/test/reporter.hpp>

#include <whirl/rpc/impl/id.hpp>
#include <await/fibers/core/id.hpp>

#include <wheels/support/compiler.hpp>

#include <random>

using namespace whirl;

//////////////////////////////////////////////////////////////////////

class PaxosClient : public ClientBase {
 public:
  PaxosClient(NodeServices services) : ClientBase(std::move(services)) {
  }

 protected:
  void MainThread() override {
    paxos::PaxosBlockingStub paxos{Channel()};

    for (size_t i = 1;; ++i) {
      consensus::Value input_value = RandomNumber(100);
      NODE_LOG("Propose({})", input_value);
      paxos.Propose(input_value);
      Threads().SleepFor(RandomNumber(1, 100));
    }
  }
};

//////////////////////////////////////////////////////////////////////

TestReporter reporter;

//////////////////////////////////////////////////////////////////////

// [3, 7]
size_t NumberOfReplicas(size_t seed) {
  return 3 + seed % 5;
}

// Seed -> simulation digest
size_t RunSimulation(size_t seed) {
  static const size_t kTimeLimit = 10000;
  static const size_t kCompletedCalls = 7;

  reporter.PrintSimSeed(seed);

  // Reset RPC and fiber ids
  await::fibers::ResetIds();
  whirl::rpc::ResetIds();

  World world{seed};

  // Cluster nodes
  auto node = MakeNode<paxos::PaxosNode>();
  world.AddServers(NumberOfReplicas(seed), node);

  // Clients
  auto client = MakeNode<PaxosClient>();
  world.AddClients(3, client);

  // Log
  std::stringstream log;
  world.WriteLogTo(log);

  // Run simulation
  world.Start();

  while (world.NumCompletedCalls() < kCompletedCalls &&
      world.TimeElapsed() < kTimeLimit) {
    world.Step();
  }
  // Stop and compute simulation digest
  size_t digest = world.Stop();

  // Print report
  reporter.PrintSimReport(world);

  // Time limit exceeded
  if (world.NumCompletedCalls() < kCompletedCalls) {
    reporter.PrintLine("Simulation {} (seed = {}) FAILED", reporter.SimIndex(), seed);
    reporter.PrintSimLog(log.str());
    reporter.Fail("Simulation time limit exceeded");
  }

  const auto history = world.History();
  reporter.DebugLine("History size: {}", history.size());

  bool safe = consensus::IsSafe(history);

  if (!safe) {
    reporter.PrintLine("Simulation {} (seed = {}) FAILED", reporter.SimIndex(), seed);
    reporter.PrintSimLog(log.str());
    reporter.PrintLine("History (seed = {}) violates SAFETY of consensus:", seed);
    whirl::histories::Print<consensus::Printer>(history, std::cout);
    reporter.Fail();
  }

  return digest;
}

//////////////////////////////////////////////////////////////////////

void RunSimulations(size_t count) {
  std::mt19937 seeds{42};

  reporter.ResetSimCount();
  reporter.PrintLine("Run simulations...");

  for (size_t i = 1; i <= count; ++i) {
    reporter.StartSim();
    RunSimulation(seeds());
    reporter.CompleteSim();
  }
}

//////////////////////////////////////////////////////////////////////

void TestDeterminism() {
  static const size_t kSeed = 104107713;

  reporter.PrintLine("Test determinism...");

  size_t digest = RunSimulation(kSeed);

  // Repeat with the same seed
  if (RunSimulation(kSeed) != digest) {
    reporter.Fail("Impl is not deterministic");
  } else {
    reporter.PrintLine("Ok");
  }
}

//////////////////////////////////////////////////////////////////////

int main() {
  TestDeterminism();
  RunSimulations(30000);

  reporter.Congratulate();

  return 0;
}
