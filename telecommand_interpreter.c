#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "telecommand_limits.h"
#include "command_list.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Input line length maximum */
#define LINE_LEN_MAX 255

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
		int (*process_arg_no)(struct cl *cl);
		int (*process_arg_one)(int arg1, struct cl *cl);
		int (*process_arg_two)(int arg1, int arg2, struct cl *cl);
	};
};

static int comp_cmd_id(const void *c1, const void *c2);

struct cmd_desc cmd_list[] = {
	{ 0, CMD_ARG_ONE, CMD_DATA_MIN,  CMD_DATA_MAX,  0,            0,            .process_arg_one = cl_process_insert_lo }, /* Insert lowest priority: cmd_id data */
	{ 1, CMD_ARG_TWO, CMD_PRIO_MIN,  CMD_PRIO_MAX,  CMD_DATA_MIN, CMD_DATA_MAX, .process_arg_two = cl_process_insert },    /* Insert: cmd_id priority data */
	{ 2, CMD_ARG_ONE, CMD_ENTRY_MIN, CMD_ENTRY_MAX, 0,            0,            .process_arg_one = cl_process_delete },    /* Delete: cmd_id entry_id */
	{ 3, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = cl_process_sort },      /* Sort: cmd_id */
	{ 4, CMD_ARG_TWO, CMD_ENTRY_MIN, CMD_ENTRY_MAX, CMD_DATA_MIN, CMD_DATA_MAX, .process_arg_two = cl_process_modify },    /* Modify: cmd_id entry_id new_data */
	{ 5, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = cl_process_print },     /* Print: cmd_id */
	{ 6, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = cl_process_reverse },   /* Reverse: cmd_id */
	{ 7, CMD_ARG_NO,  0,             0,             0,            0,            .process_arg_no  = cl_process_execute },   /* Execute: cmd_id */
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
	struct cl *command_list;
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

	command_list = cl_create();
	if (command_list == NULL) {
		fprintf(stderr, "%s: cannot create command list\n", argv[0]);
		ret = 1;
		goto end_fclose;
	}

	while(fgets(line, sizeof(line), input)) {
		if (line[strlen(line) - 1] != '\n') {
			fprintf(stderr, "%s: too long input line \"%s...\"\n",
				argv[0], line);
			ret = 1;
			goto end_cl;
		}
		line[strlen(line) - 1] = '\0';

		errno = 0;
		cmd_tmp = strtol(line, &endptr, 10);
		if ((errno != 0 && cmd_tmp == 0) ||
		    (errno == ERANGE && (cmd_tmp == LONG_MAX || cmd_tmp == LONG_MIN))) {
			fprintf(stderr, "%s: error reading command from line \"%s\": %s\n",
				argv[0], line, strerror(errno));
			ret = 1;
			goto end_cl;
		}
		if (line == endptr) {
			fprintf(stderr, "%s: error reading command from line \"%s\"\n",
				argv[0], line);
			ret = 1;
			goto end_cl;
		}

		if (cmd_tmp < CMD_ID_MIN || cmd_tmp > CMD_ID_MAX) {
			fprintf(stderr, "%s: out of bounds command %ld from line \"%s\"\n",
				argv[0], cmd_tmp, line);
			ret = 1;
			goto end_cl;
		}

		cmd_search.cmd_id = (int) cmd_tmp;
		cmd_found = bsearch(&cmd_search, cmd_list, ARRAY_SIZE(cmd_list),
				    sizeof(struct cmd_desc), comp_cmd_id);
		if (cmd_found == NULL) {
			fprintf(stderr, "%s: unexpected command %ld from line \"%s\"\n",
				argv[0], cmd_tmp, line);
			ret = 1;
			goto end_cl;
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
				goto end_cl;
			}
			if (curptr == endptr) {
				fprintf(stderr, "%s: error reading arg1 from line \"%s\"\n",
						argv[0], line);
				ret = 1;
				goto end_cl;
			}

			if (arg_tmp < cmd_found->arg1_min || arg_tmp > cmd_found->arg1_max) {
				fprintf(stderr, "%s: out of bounds arg1 %ld from line \"%s\"\n",
					argv[0], arg_tmp, line);
				ret = 1;
				goto end_cl;
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
				goto end_cl;
			}
			if (curptr == endptr) {
				fprintf(stderr, "%s: error reading arg2 from line \"%s\"\n",
						argv[0], line);
				ret = 1;
				goto end_cl;
			}

			if (arg_tmp < cmd_found->arg2_min || arg_tmp > cmd_found->arg2_max) {
				fprintf(stderr, "%s: out of bounds arg2 %ld from line \"%s\"\n",
					argv[0], arg_tmp, line);
				ret = 1;
				goto end_cl;
			}
			arg2 = (int) arg_tmp;
		}

		if (*endptr != '\0') {
			fprintf(stderr, "%s: trailing characters on line \"%s\"\n",
					argv[0], line);
			ret = 1;
			goto end_cl;
		}

		switch (cmd_found->num_arg) {
		case CMD_ARG_NO:
			ret = cmd_found->process_arg_no(command_list);
			break;
		case CMD_ARG_ONE:
			ret = cmd_found->process_arg_one(arg1, command_list);
			break;
		case CMD_ARG_TWO:
			ret = cmd_found->process_arg_two(arg1, arg2, command_list);
			break;
		}

		if (ret != 0)
			goto end_cl;
	}

end_cl:
	cl_destroy(command_list);

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
