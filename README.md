# Description 
 Mova is a stack-based VM with abilities to save, restore and transmit its own state to continuous execution from interrupted point.

## Components
* OpCodes dictionary
* Compiler
* Virtual Machine
* Serializator/Deserializator (Marhalling/Unmarshaling?)
* Hypervisor

## OpCodes dictionary
| Instruction | HEX | Description |
| ----------- | --- | ----------- |
| PUT | 0x01 | Puts `value` onto stack |
| ADD | 0x02 | Sums `value` with the latest onto stack and put result onto stack |
| GOTO | 0x03 | `offset` is an instruction offset; goes to instruction backwards onto the `offset` |
| PRINT | 0x04 | Prints the latest value onto stack |
| SLEEP | 0x05 | Sleeps for `value` in milliseconds |

## Test code
Mova bytecode representation:

```
PUT 0
ADD 1
PRINT
SLEEP 100
GOTO <ADD offset, in this case it's 3>
```


Pseudo language representation:

```
var i = 0;
loop: 
    i++;
    print(i);
    sleep(100);
```

## VM Structure
| Component | Description |
| --------- | ----------- |
| Hypervisor Listener | Listents to incomming commands from Hypervisor |
| VM | Program executor |
| State Serializator  | Serializes/Deserializes program state (Instruction Pointer, Stack) 
## Hypervisor Listener
Each program is a socket server and Hypervisor is a client that binds to server(s).

Hypervisor component into VM listens to incomming clients.

Socket port for server is calculated based on this `[PID]^34567`.

Supported commands:
| Command | Description |
| ------- | ----------- |
| save    |             |
| restore |             |

## State Serializator
### Stack Segment Structure

| Value | Description |
| ----- | -----------  |
| 0x3ACC | segment header |
| 16 bit usigned int | stack size |
| 32 bit usigned int | stack element size |

The first item in the segment is the bottom item in stack.

### Instruction Pointer Segment Structure
| Value | Description |
| ----- | -----------  |
| 0x101E | segment header |
| 32 bit usigned int | defines IP value |

### Data Segment Structure
Assume that both VMs runs the same version of the same program.


## Hypervisor Structure
| Component | Description |
| --------- | ----------- |
| IPC component | Component to communicate between two processes |
| CLI executor | Commands executor |

## Hypervisor commands
| Command | Description |
| ------- | ----------- |
| ps | get list of processes that runs Mova VM |
| bind <`PID`> | bind to program's socket
| unbind <`PID`> | unbind from program's socket
| rstate <`PID`>| read state from program |
| wstate <`PID from`> <`PID to`> | take state from one program that was fetched by `rstate` and send to another program |
