#include "stdint.h"
#include "stdio.h"

void __attribute__((cdecl)) _cstart_(uint16_t bootDrive) {

	printf("Hello %o", 0x4F9E);

	for (;;) {}
}