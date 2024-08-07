# Single-Decree Paxos

## Solving consensus

Implementation of Single-Decree-Paxos, a consensus algorithm:

- [The Part-Time Parliament](https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf)
- [Paxos Made Simple](https://lamport.azurewebsites.net/pubs/paxos-simple.pdf)
- [Formal specification in TLA+](https://github.com/tlaplus/Examples/blob/master/specifications/Paxos/Paxos.tla)

### Features

| Feature | Description |
| - | - |
| Agreement | All `Propose` calls return the same value. |
| Validity | Only values that have been proposed can be returned. |
| Termination | Every `Propose` call mush finish working at some point. |

As per [SafetyChecker](./consensus/checker.hpp).

## Roles

The Paxos algorithm distinguishes the following roles:

* _Proposer_ – an active role, coordinates the choice of a value
* _Acceptor_ - a passive role, votes for (or rejects) proposals put forward by Proposers
* _Learner_ – an optional role, tracks votes cast by Acceptors

### Notes

The Paxos algorithm requires that the grouping of acceptors be fixed (they form quorums), but allows an arbitrary number of proposers.

Each new RPC `Consensus.Propose` is a creation of a new proposer.
