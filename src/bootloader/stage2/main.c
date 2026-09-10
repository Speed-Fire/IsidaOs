#include "stdint.h"
#include "stdio.h"

void __attribute__((cdecl)) _cstart_(uint16_t bootDrive) {
	puts("Hello underworld!");
	for(;;){}
}