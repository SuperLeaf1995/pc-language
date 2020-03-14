# Potato-C
Potato-C is a project for unifying assembly into a single syntax for
many architectures, including ancient ones like CDC 6000 and SYSTEM/360.

# Example usage
```c
routine myFunc() {
	return;
}
```

# Registers (WIP)
In Potato-C registers are architecture-independent, and are not part
of any architecture to not cause confusions.
There are 2 types of registers in Potato-C, they represent the
abstracted forms of the target architecture's registers, and can be
used as a variable if there are no engough registers (i.e 6502).

## Storage registers
ra8
ra16
ra32
ra64

rb8
rb16
rb32
rb64

rc8
rc16
rc32
rc64

rd8
rd16
rd32
rd64

## Address registers
raa
rab
rac

dst
src

pp

Those registers are not part of any architecture (they are similar to x64
registers trough). They are abstracted by Potato-C into the target architecture
registers. (x86) ra8 -> al, (6502) ra8 -> x, (CDC 6000) ra8 -> X1, etc.
