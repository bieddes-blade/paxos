# Single-Decree Paxos

## Задача консенсуса

Реализация алгоритма _Single-Decree-Paxos_ для решения консенсуса:

- [The Part-Time Parliament](https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf)
- [Paxos Made Simple](https://lamport.azurewebsites.net/pubs/paxos-simple.pdf)
- [Формальная спецификация на TLA+](https://github.com/tlaplus/Examples/blob/master/specifications/Paxos/Paxos.tla)

### Свойства

| Свойство | Описание |
| - | - |
| _Agreement_ | Все вызовы `Propose` возвращают одно и то же значение. |
| _Validity_ | Возвращать можно только одно из предложенных значений. |
| _Termination_ | Каждый вызов `Propose` должен завершаться. |

См. [SafetyChecker](./consensus/checker.hpp).

## Роли

В алгоритме Paxos выделяются следующие _роли_:

* _Proposer_ – активная роль, координирует выбор значения
* _Acceptor_ - пассивная роль, голосует за (или отвергает) предложения, выдвигаемые _Proposer_-ами
* _Learner_ – опциональная роль, отслеживает отданные _Acceptor_-ами голоса

### Замечание

Алгоритм Paxos требует, чтобы группировка acceptor-ов была зафиксирована (они образуют кворумы), но допускает произвольное число proposer-ов.

Каждый новый RPC `Consensus.Propose` – это появление нового proposer-а.
