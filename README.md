# PC-Language
PC is a set of tools for making C-alike code while at the same time doing operations ala Assembly. Keeping the speed fast, and allowing the making of software for embedded devices.

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
