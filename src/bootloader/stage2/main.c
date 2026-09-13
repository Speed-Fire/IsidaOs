#include "stdint.h"
#include "stdio.h"

void __attribute__((cdecl)) _cstart_(uint16_t bootDrive) {

	printf("Hello %s%s   %d", "wor", "ld!", 55);

	for (;;) {}
}