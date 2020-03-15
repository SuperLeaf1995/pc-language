# Potato-C
Potato-C is a project for unifying assembly into a single syntax for
many architectures, including ancient ones like CDC 6000 and SYSTEM/360.

# Example usage
```c
routine myFunc() {
	x86("msg db 'Hello world!',0");
	return;
}
```

# Registers (WIP)
In Potato-C registers are architecture-independent, and are not part
of any architecture to not cause confusions.
There are 2 types of registers in Potato-C, they represent the
abstracted forms of the target architecture's registers, and can be
used as a variable if there are no engough registers (i.e 6502).

| Name | Size (Bytes) | Type |
| --- | --- | --- |
| ra8 | 1 | Storage |
| ra16 | 2 | Storage |
| ra32 | 4 | Storage |
| ra64 | 8 | Storage |
| rb8 | 1 | Storage |
| rb16 | 2 | Storage |
| rb32 | 4 | Storage |
| rb64 | 8 | Storage |
| rc8 | 1 | Storage |
| rc16 | 2 | Storage |
| rc32 | 4 | Storage |
| rc64 | 8 | Storage |
| rd8 | 1 | Storage |
| rd16 | 2 | Storage |
| rd32 | 4 | Storage |
| rd64 | 8 | Storage |
| raa | ? | Address |
| rab | ? | Address |
| rac | ? | Address |
| dst | ? | Address |
| src | ? | Address |
| pp | ? | Address |

Size of Address registers varies by architecture.

Those registers are not part of any architecture (they are similar to x64
registers trough). They are abstracted by Potato-C into the target architecture
registers. (x86) ra8 -> al, (6502) ra8 -> x, (CDC 6000) ra8 -> X1, etc.

# Social
Discord & Hoster - https://discord.gg/7AVRc4x
