#define _XOPEN_SOURCE	500

#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STRING_EMPTY(x)		((x)[0] == '\0')

static const char *argv0;

struct tokenlist {
	struct token {
		enum {
			TOKEN_LPAREN,
			TOKEN_RPAREN,
			TOKEN_NOT,
			TOKEN_AND,
			TOKEN_OR,
			TOKEN_NULL,
			TOKEN_NONNULL,
			TOKEN_STREQ,
			TOKEN_STRNEQ,
			TOKEN_EQ,
			TOKEN_NE,
			TOKEN_GE,
			TOKEN_GT,
			TOKEN_LE,
			TOKEN_LT,
			TOKEN_FILE_EF,
			TOKEN_FILE_NT,
			TOKEN_FILE_OT,
			TOKEN_BLOCK,
			TOKEN_CHAR,
			TOKEN_DIR,
			TOKEN_EXISTS,
			TOKEN_REG,
			TOKEN_SGID,
			TOKEN_GROUP,
			TOKEN_SYMLNK,
			TOKEN_USER,
			TOKEN_FIFO,
			TOKEN_READ,
			TOKEN_SIZE,
			TOKEN_SOCK,
			TOKEN_TTY,
			TOKEN_SUID,
			TOKEN_WRITE,
			TOKEN_EXEC,
			TOKEN_INT,
			TOKEN_VALUE
		} t;
		union {
			const char *str;
			long num;
		} v;
	} *array;

	size_t len;
};

struct expression {
	enum {
		EXPR_NOT,
		EXPR_BOOL,
		EXPR_STR_CMP,
		EXPR_STR_NULL,
		EXPR_INT_CMP,
		EXPR_FILE_CMP,
		EXPR_FILE_STAT
	} t;

	union {
		struct e_not {
			struct expression *a;
		} e_not;

		struct e_bool {
			struct expression *a, *b;
			enum {
				BOOL_AND,
				BOOL_OR
			} m;
		} e_bool;

		struct e_strcmp {
			const char *a, *b;
			enum {
				STR_EQ,
				STR_NE
			} m;
		} e_strcmp;

		struct e_strnul {
			const char *a;
			enum {
				STR_NUL,
				STR_VAL
			} m;
		} e_strnul;

		struct e_intcmp {
			long a, b;
			enum {
				INT_EQ,
				INT_NE,
				INT_GE,
				INT_GT,
				INT_LE,
				INT_LT
			} m;
		} e_intcmp;

		struct e_filecmp {
			const char *a, *b;
			enum {
				FCMP_EF,
				FCMP_NT,
				FCMP_OT
			} m;
		} e_filecmp;

		struct e_filestat {
			const char *a;
			enum {
				FILE_BLOCK,
				FILE_CHAR,
				FILE_DIR,
				FILE_EXIST,
				FILE_REG,
				FILE_SGID,
				FILE_GROUP,
				FILE_STCKY,
				FILE_SYMLNK,
				FILE_USER,
				FILE_FIFO,
				FILE_READ,
				FILE_SIZE,
				FILE_SOCK,
				FILE_TTY,
				FILE_SUID,
				FILE_WRITE,
				FILE_EXEC
			} m;
		} e_filestat;
	} u;
};

static void parse_tokens(struct tokenlist *tokens, int argc, const char *argv[]);
static void build_tree(struct expression *expr, const struct tokenlist *tokens);
static bool eval(const struct expression *expr);

static bool is_bracket(void)
{
	const char *base;
	char *str2;
	bool ret;

	str2 = strdup(argv0);
	if (!str2) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			argv0, strerror(errno));
		exit(2);
	}
	base = basename(str2);
	ret = !strcmp(base, "[");
	free(str2);
	return ret;
}

static bool isnum(const char *str)
{
	if (str[0] == '-') {
		str++;
	}
	while (*str) {
		if (!isdigit(*str)) {
			return false;
		}
		str++;
	}
	return true;
}

static void init_tokens(struct tokenlist *tokens, int argc)
{
	tokens->len = argc - 1;
	tokens->array = malloc(sizeof(struct token) * tokens->len);
	if (!tokens->array) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			argv0, strerror(errno));
		exit(2);
	}
}

static void parse_tokens(struct tokenlist *tokens, int argc, const char *argv[])
{
	struct token *token;
	int i;

	for (i = 1; i < argc; i++) {
		token = &tokens->array[i - 1];

		if (!strcmp(argv[i], "(")) {
			token->t = TOKEN_LPAREN;
		} else if (!strcmp(argv[i], ")")) {
			token->t = TOKEN_RPAREN;
		} else if (!strcmp(argv[i], "!")) {
			token->t = TOKEN_NOT;
		} else if (!strcmp(argv[i], "=") || !strcmp(argv[i], "==")) {
			token->t = TOKEN_STREQ;
		} else if (!strcmp(argv[i], "!=")) {
			token->t = TOKEN_STRNEQ;
		} else if (!strcmp(argv[i], "-eq")) {
			token->t = TOKEN_EQ;
		} else if (!strcmp(argv[i], "-ne")) {
			token->t = TOKEN_NE;
		} else if (!strcmp(argv[i], "-ge")) {
			token->t = TOKEN_GE;
		} else if (!strcmp(argv[i], "-gt")) {
			token->t = TOKEN_GT;
		} else if (!strcmp(argv[i], "-le")) {
			token->t = TOKEN_LE;
		} else if (!strcmp(argv[i], "-lt")) {
			token->t = TOKEN_LT;
		} else if (!strcmp(argv[i], "-ef")) {
			token->t = TOKEN_FILE_EF;
		} else if (!strcmp(argv[i], "-nt")) {
			token->t = TOKEN_FILE_NT;
		} else if (!strcmp(argv[i], "-ot")) {
			token->t = TOKEN_FILE_OT;
		} else if (isnum(argv[i])) {
			token->t = TOKEN_INT;
			token->v.num = atol(argv[i]);
		} else if (argv[i][0] != '-') {
			token->t = TOKEN_VALUE;
			token->v.str = argv[i];
		} else {
			if (strlen(argv[i]) != 2) {
				fprintf(stderr, "%s: unknown condition: %s\n",
					argv0, argv[i]);
				exit(2);
			}

			switch (argv[i][1]) {
			case 'a':
				token->t = TOKEN_AND;
				break;
			case 'o':
				token->t = TOKEN_OR;
				break;
			case 'n':
				token->t = TOKEN_NONNULL;
				break;
			case 'z':
				token->t = TOKEN_NULL;
				break;
			case 'b':
				token->t = TOKEN_BLOCK;
				break;
			case 'c':
				token->t = TOKEN_CHAR;
				break;
			case 'd':
				token->t = TOKEN_DIR;
				break;
			case 'e':
				token->t = TOKEN_EXISTS;
				break;
			case 'f':
				token->t = TOKEN_REG;
				break;
			case 'g':
				token->t = TOKEN_SGID;
				break;
			case 'G':
				token->t = TOKEN_GROUP;
				break;
			case 'h':
			case 'L':
				token->t = TOKEN_SYMLNK;
				break;
			case 'O':
				token->t = TOKEN_USER;
				break;
			case 'p':
				token->t = TOKEN_FIFO;
				break;
			case 'r':
				token->t = TOKEN_READ;
				break;
			case 's':
				token->t = TOKEN_SIZE;
				break;
			case 'S':
				token->t = TOKEN_SOCK;
				break;
			case 't':
				token->t = TOKEN_TTY;
				break;
			case 'u':
				token->t = TOKEN_SUID;
				break;
			case 'w':
				token->t = TOKEN_WRITE;
				break;
			case 'x':
				token->t = TOKEN_EXEC;
				break;
			default:
				fprintf(stderr, "%s: unknown condition: %s\n",
					argv0, argv[i]);
				exit(2);
			}
		}
	}
}

static void build_tree(struct expression *expr, const struct tokenlist *tokens)
{
	size_t i;

	for (i = 0; i < tokens->len; i++) {
		const struct token *token;

		token = &tokens->array[i];
		if (token->t == TOKEN_LPAREN) {
			struct tokenlist subtokens;
			size_t j;

			for (j = 1; j < tokens->len; j++) {
				if (tokens->array[j].t == TOKEN_RPAREN) {
					break;
				}
			}
			if (j == tokens->len) {
				fprintf(stderr, "%s: missing ')'\n", argv0);
				exit(2);
			}
			subtokens.len = j - i;
		}
	}
}

static bool eval_bool(const struct e_bool *expr)
{
	/* Simulates short-circuiting */
	switch (expr->m) {
	case BOOL_AND:
		if (!eval(expr->a)) {
			return 0;
		}
		return eval(expr->b);
	case BOOL_OR:
		if (eval(expr->a)) {
			return 1;
		}
		return eval(expr->b);
	}
}

static bool eval_strcmp(const struct e_strcmp *expr)
{
	int ret;

	ret = strcmp(expr->a, expr->b);
	switch (expr->m) {
	case STR_EQ:
		return !ret;
	case STR_NE:
		return ret;
	}
}

static bool eval_intcmp(const struct e_intcmp *expr)
{
	switch (expr->m) {
	case INT_EQ:
		return expr->a == expr->b;
	case INT_NE:
		return expr->a != expr->b;
	case INT_GE:
		return expr->a >= expr->b;
	case INT_GT:
		return expr->a > expr->b;
	case INT_LE:
		return expr->a <= expr->b;
	case INT_LT:
		return expr->a < expr->b;
	}
}

static bool eval_filecmp(const struct e_filecmp *expr)
{
	struct stat stbuf_a, stbuf_b;

	if (stat(expr->a, &stbuf_a)) {
		exit(1);
	}
	if (stat(expr->b, &stbuf_a)) {
		exit(1);
	}

	switch (expr->m) {
	case FCMP_EF:
		return stbuf_a.st_ino == stbuf_b.st_ino
			&& stbuf_a.st_dev == stbuf_b.st_dev;
	case FCMP_NT:
		return stbuf_a.st_mtime > stbuf_b.st_mtime;
	case FCMP_OT:
		return stbuf_a.st_mtime < stbuf_b.st_mtime;
	}
}

static bool eval_filestat(const struct e_filestat *expr)
{
	struct stat stbuf;

	/* Cases that don't need a stat() */
	switch (expr->m) {
	case FILE_TTY: {
		char *ptr;
		int fd;

		fd = strtol(expr->a, &ptr, 10);
		if (*ptr || fd < 0) {
			return false;
		}
		return isatty(fd);
	}
	case FILE_EXIST:
		return !access(expr->a, F_OK);
	case FILE_READ:
		return !access(expr->a, R_OK);
	case FILE_WRITE:
		return !access(expr->a, W_OK);
	case FILE_EXEC:
		return !access(expr->a, X_OK);
	default:
		break;
	}

	/* lstat() or stat() */
	if (expr->m == FILE_SYMLNK) {
		if (lstat(expr->a, &stbuf)) {
			exit(1);
		}
	} else {
		if (stat(expr->a, &stbuf)) {
			exit(1);
		}
	}

	/* Actual check */
	switch (expr->m) {
	case FILE_BLOCK:
		return S_ISBLK(stbuf.st_mode);
	case FILE_CHAR:
		return S_ISCHR(stbuf.st_mode);
	case FILE_DIR:
		return S_ISDIR(stbuf.st_mode);
	case FILE_REG:
		return S_ISREG(stbuf.st_mode);
	case FILE_SGID:
		return stbuf.st_mode & S_ISGID;
	case FILE_STCKY:
		return stbuf.st_mode & S_ISVTX;
	case FILE_SYMLNK:
		return S_ISLNK(stbuf.st_mode);
	case FILE_USER:
		return stbuf.st_uid == geteuid();
	case FILE_FIFO:
		return S_ISFIFO(stbuf.st_mode);
	case FILE_SIZE:
		return stbuf.st_size > 0;
	case FILE_SOCK:
		return S_ISSOCK(stbuf.st_mode);
	case FILE_SUID:
		return stbuf.st_mode & S_ISUID;

	/* Should've already been covered */
	case FILE_EXIST:
	case FILE_READ:
	case FILE_WRITE:
	case FILE_EXEC:
	case FILE_TTY:
	default:
		abort();
	}
}

static bool eval(const struct expression *expr)
{
	switch (expr->t) {
	case EXPR_NOT:
		return !eval(expr->u.e_not.a);
	case EXPR_BOOL:
		return eval_bool(&expr->u.e_bool);
	case EXPR_STR_CMP:
		return eval_strcmp(&expr->u.e_strcmp);
	case EXPR_STR_NULL:
		return STRING_EMPTY(expr->u.e_strnul.a);
	case EXPR_INT_CMP:
		return eval_intcmp(&expr->u.e_intcmp);
	case EXPR_FILE_CMP:
		return eval_filecmp(&expr->u.e_filecmp);
	case EXPR_FILE_STAT:
		return eval_filestat(&expr->u.e_filestat);
	}
}

/*
 * Usage: [ EXPRESSION ]
 *
 * Supported expressions:
 * -n STRING
 * -z STRING
 * STRING1 = STRING2
 * STRING1 == STRING2
 * STRING1 != STRING2
 */
int main(int argc, const char *argv[])
{
	struct tokenlist tokens;
	struct expression tree;

	argv0 = argv[0];
	if (is_bracket()) {
		if (strcmp(argv[--argc], "]")) {
			fprintf(stderr, "%s: missing ']'\n", argv[0]);
			return 2;
		}
	}
	init_tokens(&tokens, argc);
	parse_tokens(&tokens, argc, argv);
	build_tree(&tree, &tokens);
	return 0;
}
