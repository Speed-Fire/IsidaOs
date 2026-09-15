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

#define ANY_SYMBOL_STATE 0
#define CONTROL_STATE 1
#define CONTROL_PARAMETER_STATE 2
#define CONTROL_NUM_LENGTH_STATE 3
#define CONTROL_NUM_LENGTH_SHORT_STATE 4
#define CONTROL_NUM_LENGTH_LONG_STATE 5
#define CONTROL_FINAL_STATE 6

#define NUM_LENGTH_DEFAULT 0
#define NUM_LENGTH_SHORT_SHORT 1
#define NUM_LENGTH_SHORT 2
#define NUM_LENGTH_LONG 3
#define NUM_LENGTH_LONG_LONG 4

void printString(const char* str, int length);
int* printInt(int* argp, int length, int radix, bool sign);

void printf(const char* fmt, ...) {
	int* argp = (int*)&fmt;
	argp += sizeof(fmt) / sizeof(int);

	int global_state = ANY_SYMBOL_STATE;
	bool hasParameter = false;
	int parameter = 0;
	int length = NUM_LENGTH_DEFAULT;

	while (*fmt) {
		switch (global_state) {
		case ANY_SYMBOL_STATE:

			if (*fmt == '%') {
				global_state = CONTROL_STATE;
			}
			else {
				putc(*fmt);
			}

			break;

		case CONTROL_STATE:

			switch (*fmt) {
			case '.': global_state = CONTROL_PARAMETER_STATE;
					  hasParameter = true;
					  break;

			default:  goto CONTROL_NUM_LENGTH_STATE_;
			}

			break;

		case CONTROL_PARAMETER_STATE:
			if (*fmt >= '0' && *fmt <= '9') {
				parameter *= 10;
				parameter += *fmt - '0';
			}
			else {
				goto CONTROL_NUM_LENGTH_STATE_;
			}
			break;

		case CONTROL_NUM_LENGTH_STATE:
			CONTROL_NUM_LENGTH_STATE_:
			switch (*fmt) {
			case 'h': global_state = CONTROL_NUM_LENGTH_SHORT_STATE;
					  length = NUM_LENGTH_SHORT;
					  break;

			case 'l': global_state = CONTROL_NUM_LENGTH_LONG_STATE;
					  length = NUM_LENGTH_LONG;
					  break;

			default: goto CONTROL_FINAL_STATE_;
			}

		case CONTROL_NUM_LENGTH_SHORT_STATE:

			switch (*fmt) {

			case 'h': global_state = CONTROL_FINAL_STATE;
					  length = NUM_LENGTH_SHORT_SHORT;
					  break;

			default: goto CONTROL_FINAL_STATE_;
			}

			break;

		case CONTROL_NUM_LENGTH_LONG_STATE:

			switch (*fmt) {

			case 'l': global_state = CONTROL_FINAL_STATE;
					  length = NUM_LENGTH_LONG_LONG;
					  break;

			default: goto CONTROL_FINAL_STATE_;
			}

			break;

		case CONTROL_FINAL_STATE:
			CONTROL_FINAL_STATE_:
			switch (*fmt) {
			case '%': putc('%');
					  break;

			case 'c':;char c = *((char*)argp);
					  argp += sizeof(c) / sizeof(int);
					  putc(c);
					  break;

			case 's':;const char* str = (const char*)*argp;
					  argp += sizeof(str) / sizeof(int);
					  if (hasParameter)
						  printString(str, parameter);
					  else
						  printString(str, -1);
					  break;

			case 'i':
			case 'd': argp = printInt(argp, length, 10, true);
					  break;

			case 'u': argp = printInt(argp, length, 10, false);
					  break;

			case 'x':
			case 'X':
			case 'p': argp = printInt(argp, length, 16, false);
					  break;

			case 'o': argp = printInt(argp, length, 8, false);
					  break;

			default:  break;
			}

			global_state = ANY_SYMBOL_STATE;
			hasParameter = false;
			parameter = 0;
			length = NUM_LENGTH_DEFAULT;

			break;
		}

		fmt++;
	}
}

void printString(const char* str, int length) {
	while (*str && (length != 0 || length == -1)) {
		putc(*str);
		str++;
		length--;
	}
}

const char g_HexChars[] = "0123456789ABCDEF";

int* printInt(int* argp, int length, int radix, bool sign) {
	char buffer[32];
	unsigned long long number;
	int num_sign = 1;
	int pos = 0;

	switch (length) {
	case NUM_LENGTH_SHORT_SHORT:
	case NUM_LENGTH_SHORT:
	case NUM_LENGTH_DEFAULT:
		if (sign) {
			int num = *argp;
			if (num < 0) {
				num = -num;
				num_sign = -1;
			}
			number = (unsigned long long)num;
		}
		else {
			number = *(unsigned int*)argp;
		}
		argp++;
		break;

	case NUM_LENGTH_LONG:
		if (sign) {
			long num = *(long*)argp;
			if (num < 0) {
				num = -num;
				num_sign = -1;
			}
			number = (unsigned long long)num;
		}
		else {
			number = *(unsigned long*)argp;
		}
		argp += sizeof(long) / sizeof(int);
		break;

	case NUM_LENGTH_LONG_LONG:
		if (sign) {
			long num = *(long long*)argp;
			if (num < 0) {
				num = -num;
				num_sign = -1;
			}
			number = (unsigned long long)num;
		}
		else {
			number = *(unsigned long long*)argp;
		}
		argp += sizeof(long long) / sizeof(int);
		break;
	}

	if (number == 0) {
		putc('0');
		return argp;
	}

	while (number > 0) {
		uint32_t rem = 0;
		_x86_Div64_32(number, radix, &number, &rem);
		buffer[pos] = g_HexChars[rem];
		pos++;
	}

	if (sign && num_sign < 0)
	{
		buffer[pos] = '-';
		pos++;
	}

	for (; pos >= 0; pos--)
		putc(buffer[pos]);

	return argp;
}