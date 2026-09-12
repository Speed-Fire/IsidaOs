#include "stdint.h"
#include "stdio.h"

void __attribute__((cdecl)) _cstart_(uint16_t bootDrive) {

	uint64_t a = 10;
	uint64_t b = 10;
	uint64_t c = a / b;

	printf("Hello %s%s   %d", "wor", "ld!", c);

	for (;;) {}
}