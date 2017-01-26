#define _XOPEN_SOURCE	500

#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>

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
			TOKEN_STRCMP,
			TOKEN_INTCMP,
			TOKEN_FILECMP,
			TOKEN_FILESTAT,
			TOKEN_VALUE
		} t;
		const char *str;
	} *array;

	size_t len;
	size_t capacity;
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
	if (str2) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			argv0, strerror(errno));
		exit(-1);
	}
	base = basename(str2);
	ret = !strcmp(base, "[");
	free(str2);
	return ret;
}

static void parse_tokens(struct tokenlist *tokens, int argc, const char *argv[])
{
	/* TODO */
}

static void build_tree(struct expression *expr, const struct tokenlist *tokens)
{
	/* TODO */
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
	parse_tokens(&tokens, argc, argv);
	build_tree(&tree, &tokens);
	return 0;
}
