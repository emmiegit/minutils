#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TOLOWER(x)		((x) | 0x20)

static const char *argv0;

struct format {
	const char *style;
	enum {
		FMT_DECIMAL,
		FMT_UNSIGNED,
		FMT_FLOAT,
		FMT_CHAR,
		FMT_STRING,
		FMT_INTERP, /* %b */
		FMT_POSIX   /* %q */
	} type;
	union {
		signed long n;
		unsigned long u;
		double f;
		char c;
		const char *s;
	} value;
};

static unsigned char read_octal(const char **str, size_t len)
{
	unsigned char sum;
	size_t i;
	char ch;

	sum = 0;
	for (i = 0; i < len; i++) {
		ch = (*str)[i];
		if ('0' > ch || ch > '7')
			break;

		sum <<= 3;
		sum += ch - '0';
		(*str)++;
	}
	return sum;
}

static uint_least32_t read_hex(const char **str, size_t len)
{
	uint_least32_t sum;
	size_t i;
	char ch;

	sum = 0;
	for (i = 0; i < len; i++) {
		ch = (*str)[i];
		if (!isxdigit(ch))
			break;

		sum <<= 4;
		if (isdigit(ch))
			sum += ch - '0';
		else
			sum += TOLOWER(ch) - 'a';
	}
	return sum;
}

static void print_escape(const char *str)
{
	union {
		uint_least32_t num;
		char ch;
	} u;

	switch (*str) {
	case '\"':
	case '\'':
	case '\\':
		putchar(*str);
		break;
	case 'a':
		putchar('\a');
		break;
	case 'b':
		putchar('\b');
		break;
	case 'c':
		exit(0);
	case 'e':
		putchar('\033');
		break;
	case 'f':
		putchar('\f');
		break;
	case 'n':
		putchar('\n');
		break;
	case 'r':
		putchar('\r');
		break;
	case 't':
		putchar('\t');
		break;
	case 'v':
		putchar('\v');
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		u.ch = read_octal(&str, 3);
		putchar(u.ch);
		break;
	case 'x':
		u.ch = read_hex(&str, 2);
		putchar(u.ch);
		break;
	case 'u':
		u.num = read_hex(&str, 4);
		printf("[TODO unicode %4x]", u.num);
		break;
	case 'U':
		u.num = read_hex(&str, 8);
		printf("[TODO unicode %8x]", u.num);
		break;
	default:
		putchar('\\');
		putchar(*str);
	}
}

static void copy_format(struct format *fmt, const char *str)
{
	static char buf[32];
	size_t i;

	fmt->style = buf;

	for (i = 0; str[i] && i < sizeof(buf) - 2; i++) {
		switch (str[i]) {
		case '\'':
		case '#':
		case '-':
		case '+':
		case ' ':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			buf[i + 1] = str[i];
			break;
		case 'd':
		case 'i':
		case 'o':
			fmt->type = FMT_DECIMAL;
			goto end;
		case 'u':
		case 'x':
		case 'X':
			fmt->type = FMT_UNSIGNED;
			goto end;
		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			fmt->type = FMT_FLOAT;
			goto end;
		case 'c':
			fmt->type = FMT_CHAR;
			goto end;
		case 's':
			fmt->type = FMT_STRING;
			goto end;
		case 'b':
			fmt->type = FMT_INTERP;
			goto end;
		case 'q':
			fmt->type = FMT_POSIX;
			goto end;
		default:
			fprintf(stderr, "%s: %%%c: Invalid directive\n",
				argv0, str[i]);
			exit(1);
		}
	}
	if (i == sizeof(buf) - 2) {
		fprintf(stderr, "%s: format string too long\n",
			argv0);
		exit(1);
	}

end:
	buf[0] = '%';
	buf[i + 1] = str[i];
	buf[i + 2] = '\0';
}

static void print_format(const struct format *fmt)
{
printf(":%s:\n", fmt->style);

	/* TODO */
}

int main(int argc, const char *argv[])
{
	struct format fmt;
	const char *str;

	if (argc < 2) {
		fprintf(stderr, "%s: missing format string\n",
			argv[0]);
		return 1;
	}

	argv0 = argv[0];
	str = argv[1];

	while (*str++) {
		switch (*str) {
		case '\\':
			print_escape(++str);
			if (!*str)
				return 0;
			break;
		case '%':
			str++;
			if (*str == '%') {
				putchar('%');
				break;
			}
			copy_format(&fmt, ++str);
			print_format(&fmt);
			break;
		default:
			putchar(*str);
		}
	}
	return 0;
}
