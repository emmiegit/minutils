#define _XOPEN_SOURCE	500

#include <libgen.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	const char *argv0;

} opt;

struct expression {
	enum {
		EXPR_NOT,
		EXPR_BOOL,
		EXPR_STR_CMP,
		EXPR_STR_NULL,
		EXPR_INT_CMP,
		EXPR_FILE_CMP,
		EXPR_FILE
	} t;

	union {
		struct {
			struct expression *a;
		} e_not;

		struct {
			struct expression *a, *b;
			enum {
				BOOL_AND,
				BOOL_OR
			} m;
		} e_bool;

		struct {
			const char *a, *b;
			enum {
				STR_EQ,
				STR_NE
			} m;
		} e_strcmp;

		struct {
			const char *a;
			enum {
				STR_NUL,
				STR_VAL
			} m;
		} e_strnul;

		struct {
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

		struct {
			const char *a, *b;
			enum {
				FCMP_EF,
				FCMP_NT,
				FCMP_OT
			} m;
		} e_filecmp;

		struct {
			const char *a;
			enum {
				FILE_BLOCK,
				FILE_CHAR,
				FILE_DIR,
				FILE_EXIST,
				FILE_REG,
				FILE_SGID,
				FILE_GROUP,
				FILE_STVX,
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
		} e_file;
	} u;
};

static int is_bracket(void)
{
	const char *base;
	char *dup;
	int ret;

	dup = strdup(opt.argv0);
	if (!dup) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			opt.argv0, strerror(errno));
		exit(-1);
	}
	base = basename(dup);
	ret = !strcmp(base, "[");
	free(dup);
	return ret;
}

static void build_tree(struct expressoin *tree, int argc, const char *argv[])
{
	/* TODO */
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
	struct expression tree;

	opt.argv0 = argv[0];
	if (is_bracket()) {
		if (strcmp(argv[--argc], "]")) {
			fprintf(stderr, "%s: missing ']'\n", argv[0]);
			return 1;
		}
	}
	build_tree(&tree, argc, argv);
	return 0;
}
