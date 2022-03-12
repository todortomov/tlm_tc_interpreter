#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Input line length maximum */
#define LINE_LEN_MAX 255

/* Command id minimum. Supported values: [INT_MIN..CMD_ID_MAX] */
#define CMD_ID_MIN 0
/* Command id maximum. Supported values: [CMD_ID_MIN..INT_MAX] */
#define CMD_ID_MAX 100

/* Command priority minimum. Supported values: [INT_MIN..CMD_PRIO_MAX] */
#define CMD_PRIO_MIN 0
/* Command priority maximum. Supported values: [CMD_PRIO_MIN..INT_MAX] */
#define CMD_PRIO_MAX 255

/* Command data minimum. Supported values: [INT_MIN..CMD_DATA_MAX] */
#define CMD_DATA_MIN 0
/* Command data maximum. Supported values: [CMD_DATA_MIN..INT_MAX] */
#define CMD_DATA_MAX INT_MAX

/* Command entry id minimum. Supported values: [INT_MIN..CMD_ENTRY_MAX] */
#define CMD_ENTRY_MIN 0
/* Command entry id maximum. Supported values: [CMD_ENTRY_MIN..INT_MAX] */
#define CMD_ENTRY_MAX INT_MAX

enum cmd_arg_num {
	CMD_ARG_NO,
	CMD_ARG_ONE,
	CMD_ARG_TWO
};

struct cmd_desc {
	int cmd_id;
	enum cmd_arg_num num_arg;
	int arg1_min;
	int arg1_max;
	int arg2_min;
	int arg2_max;
	union {
		int (*process_arg_no)(void *queue);
		int (*process_arg_one)(int arg1, void *queue);
		int (*process_arg_two)(int arg1, int arg2, void *queue);
	};
};

static int comp_cmd_id(const void *c1, const void *c2);

int process_insert_lo(int data, void *queue);
int process_insert(int prio, int data, void *queue);
int process_delete(int entry_id, void *queue);
int process_sort(void *queue);
int process_modify(int entry_id, int data, void *queue);
int process_print(void *queue);
int process_reverse(void *queue);
int process_execute(void *queue);

struct cmd_desc cmd_list[] = {
	{ 0, CMD_ARG_ONE, CMD_DATA_MIN,  CMD_DATA_MAX,  0,            0,            .process_arg_one = process_insert_lo }, /* Insert lowest priority: cmd_id data */
	{ 1, CMD_ARG_TWO, CMD_PRIO_MIN,  CMD_PRIO_MAX,  CMD_DATA_MIN, CMD_DATA_MAX, .process_arg_two = process_insert },    /* Insert: cmd_id priority data */
	{ 2, CMD_ARG_ONE, CMD_ENTRY_MIN, CMD_ENTRY_MAX, 0,            0,            .process_arg_one = process_delete },    /* Delete: cmd_id entry_id */
	{ 3, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = process_sort },      /* Sort: cmd_id */
	{ 4, CMD_ARG_TWO, CMD_ENTRY_MIN, CMD_ENTRY_MAX, CMD_DATA_MIN, CMD_DATA_MAX, .process_arg_two = process_modify },    /* Modify: cmd_id entry_id new_data */
	{ 5, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = process_print },     /* Print: cmd_id */
	{ 6, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = process_reverse },   /* Reverse: cmd_id */
	{ 7, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = process_execute },   /* Execute: cmd_id */
};

int main(int argc, char *argv[])
{
	FILE *input;
	char line[LINE_LEN_MAX];
	long cmd_tmp, arg_tmp;
	int arg1, arg2;
	struct cmd_desc cmd_search;
	struct cmd_desc *cmd_found;
	char *endptr, *curptr;
	int ret = 0;

	if (argc != 3 || strcmp(argv[1], "-i")) {
		fprintf(stderr, "usage: %s -i batch_file\n", argv[0]);
		ret = 1;
		goto end;
	}

	input = fopen(argv[2], "r");
	if (input == NULL) {
		fprintf(stderr, "%s: error opening file \"%s\": %s\n",
			argv[0], argv[2], strerror(errno));
		ret = 1;
		goto end;
	}

	while(fgets(line, sizeof(line), input)) {
		if (line[strlen(line) - 1] != '\n') {
			fprintf(stderr, "%s: too long input line \"%s...\"\n",
				argv[0], line);
			ret = 1;
			goto end_fclose;
		}
		line[strlen(line) - 1] = '\0';

		errno = 0;
		cmd_tmp = strtol(line, &endptr, 10);
		if ((errno != 0 && cmd_tmp == 0) ||
		    (errno == ERANGE && (cmd_tmp == LONG_MAX || cmd_tmp == LONG_MIN))) {
			fprintf(stderr, "%s: error reading command from line \"%s\": %s\n",
				argv[0], line, strerror(errno));
			ret = 1;
			goto end_fclose;
		}
		if (line == endptr) {
			fprintf(stderr, "%s: error reading command from line \"%s\"\n",
				argv[0], line);
			ret = 1;
			goto end_fclose;
		}

		if (cmd_tmp < CMD_ID_MIN || cmd_tmp > CMD_ID_MAX) {
			fprintf(stderr, "%s: out of bounds command %ld from line \"%s\"\n",
				argv[0], cmd_tmp, line);
			ret = 1;
			goto end_fclose;
		}

		cmd_search.cmd_id = (int) cmd_tmp;
		cmd_found = bsearch(&cmd_search, cmd_list, ARRAY_SIZE(cmd_list),
				    sizeof(struct cmd_desc), comp_cmd_id);
		if (cmd_found == NULL) {
			fprintf(stderr, "%s: unexpected command %ld from line \"%s\"\n",
				argv[0], cmd_tmp, line);
			ret = 1;
			goto end_fclose;
		}

		if (cmd_found->num_arg == CMD_ARG_ONE ||
		    cmd_found->num_arg == CMD_ARG_TWO) {
			curptr = endptr;
			errno = 0;
			arg_tmp = strtol(curptr, &endptr, 10);
			if ((errno != 0 && arg_tmp == 0) ||
			    (errno == ERANGE && (arg_tmp == LONG_MAX || arg_tmp == LONG_MIN))) {
				fprintf(stderr, "%s: error reading arg1 from line \"%s\": %s\n",
						argv[0], line, strerror(errno));
				ret = 1;
				goto end_fclose;
			}
			if (curptr == endptr) {
				fprintf(stderr, "%s: error reading arg1 from line \"%s\"\n",
						argv[0], line);
				ret = 1;
				goto end_fclose;
			}

			if (arg_tmp < cmd_found->arg1_min || arg_tmp > cmd_found->arg1_max) {
				fprintf(stderr, "%s: out of bounds arg1 %ld from line \"%s\"\n",
					argv[0], arg_tmp, line);
				ret = 1;
				goto end_fclose;
			}
			arg1 = (int) arg_tmp;
		}

		if (cmd_found->num_arg == CMD_ARG_TWO) {
			curptr = endptr;
			errno = 0;
			arg_tmp = strtol(curptr, &endptr, 10);
			if ((errno != 0 && arg_tmp == 0) ||
			    (errno == ERANGE && (arg_tmp == LONG_MAX || arg_tmp == LONG_MIN))) {
				fprintf(stderr, "%s: error reading arg2 from line \"%s\": %s\n",
						argv[0], line, strerror(errno));
				ret = 1;
				goto end_fclose;
			}
			if (curptr == endptr) {
				fprintf(stderr, "%s: error reading arg2 from line \"%s\"\n",
						argv[0], line);
				ret = 1;
				goto end_fclose;
			}

			if (arg_tmp < cmd_found->arg2_min || arg_tmp > cmd_found->arg2_max) {
				fprintf(stderr, "%s: out of bounds arg2 %ld from line \"%s\"\n",
					argv[0], arg_tmp, line);
				ret = 1;
				goto end_fclose;
			}
			arg2 = (int) arg_tmp;
		}

		if (*endptr != '\0') {
			fprintf(stderr, "%s: trailing characters on line \"%s\"\n",
					argv[0], line);
			ret = 1;
			goto end_fclose;
		}
	}

end_fclose:
	fclose(input);

end:
	return ret;
}

static int comp_cmd_id(const void *c1, const void *c2)
{
	struct cmd_desc *cmd1 = (struct cmd_desc *) c1;
	struct cmd_desc *cmd2 = (struct cmd_desc *) c2;

	return cmd1->cmd_id - cmd2->cmd_id;
}

int process_insert_lo(int data, void *queue)
{
	return 0;
}

int process_insert(int prio, int data, void *queue)
{
	return 0;
}

int process_delete(int entry_id, void *queue)
{
	return 0;
}

int process_sort(void *queue)
{
	return 0;
}

int process_modify(int entry_id, int data, void *queue)
{
	return 0;
}

int process_print(void *queue)
{
	return 0;
}

int process_reverse(void *queue)
{
	return 0;
}

int process_execute(void *queue)
{
	return 0;
}
