# Potato-C
Potato-C is a project for unifying assembly into a single syntax for
many architectures, including ancient ones like CDC 6000 and SYSTEM/360.

# Example usage
```c
#include <kernel_lib>
#define __HELLO_WORLD__ Hello World

routine myFunc() {
	ra8 = kstrlen(__HELLO_WORLD__);
	kprint(__HELLO_WORLD__,ra8);
	return;
}
```

# Social
Discord & Hoster - https://discord.gg/7AVRc4x
