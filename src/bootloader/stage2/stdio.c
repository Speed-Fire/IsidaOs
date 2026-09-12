#include "stdio.h"
#include "x86.h"
#include "stdint.h"

void putc(char c) {
	_x86_Video_WriteCharTeletype(c, 0);
}

void puts(const char* str) {
	while (*str) {
		putc(*str);
		str++;
	}
}

int printf(const char* fmt, ...) {
	int* argp = (int*)&fmt;
	argp += sizeof(fmt) / sizeof(int);

	int length = -1;
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;

			if (*fmt >= '0' && *fmt <= '9') {
				length = (*fmt) - '0';
				fmt++;
				while (*fmt >= '0' && *fmt <= '9') {
					length *= 10;
					length += (*fmt) - '0';
					fmt++;
				}
			}

			switch (*fmt) {
			case 'd':
			case 'i':;
				int d = *argp;
				argp++;

				if (d < 0) {
					putc('-');
					d *= -1;
				}
				if (d == 0) {
					putc('0');
					break;
				}

				int tailingZeroes = 0;
				while (d % 10 == 0) {
					d /= 10;
					tailingZeroes++;
				}

				int d2 = 0;
				do {
					d2 *= 10;
					d2 += d % 10;
					d /= 10;
				} while (d > 0);

				while (d2 > 0) {
					putc(d2 % 10 + '0');
					d2 /= 10;
				}
				for (;tailingZeroes > 0; tailingZeroes--) putc('0');

				break;

			case 'f':;
				double f = *((double*)argp);
				argp += sizeof(f) / sizeof(int);
				break;

			case 'c':;
				char c = *((char*)argp);
				argp += sizeof(c) / sizeof(int);

				putc(c);

				break;

			case 's':;
				const char* str = (const char*)*argp;
				argp += sizeof(str) / sizeof(int);

				while (*str && length != 0) {
					putc(*str);
					str++;
					length--;
				}

				break;
			}
			fmt++;

			length = -1;
		}
		else {
			putc(*fmt);
			fmt++;
		}
	}

	return 0;
}