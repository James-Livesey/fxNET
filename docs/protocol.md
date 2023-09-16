# fxNET Protocol
The fxNET Protocol defines the commands and data formats communicated over serial between calculators and fxNET modules.

## Conventions
### Header start
All headers from the calculator start with `FD 66 78 00`, where the last byte is the version number of the protocol.

Similarly, the module sends headers that start with `FE 66 78 00`.

The next byte after the version number is the message ID, which allows responses to messages to be tracked asynchronously. After the message ID is the command byte.

All communications end with `FF`, and the byte preceeding `FF` is the checksum from FE.

### Command list
Here is a list of commands that can be used via the protocol.

| Command | Issued by | Arguments | Description |
|-|-|-|-|
| `01` | Both | Byte-counted payload | Received message acknowledged |
| `02` | Both | None | Checksum error (send again) |
| `03` | Both | Byte-counted string | Processing error |
| `04` | Module | None | Hello confirmation |
| `05` | Calculator | None | fxNET is exiting |

### Byte-counted strings and payloads
Byte-counted payloads start with a 16-bit unsigned integer that represents the length of the payload. The payload is then sent.

Strings are the same as payloads (without null termination) but respresent textual information.

## Startup
| Calculator | Module | Description |
|-|-|-|
| `FF FF FF FF` | | Wake up module if it is sleeping |
| | `FE 66 78 00 00 04 E1 FF` | Send hello confirmation |
| `FD 66 78 00 01 01 00 00 DE FF` | | Acknowledge confirmation |

## fxNET add-in exit
| Calculator | Module | Description |
|-|-|-|
| `FD 66 78 00 00 05 E1 FF` | | fxNET is exiting |
| | `FE 66 78 00 00 01 00 00 DE FF` | Acknowledge (module may sleep if needed) |