#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Input line length maximum */
#define LINE_LEN_MAX 255

/* Command id minimum */
#define CMD_ID_MIN 0
/* Command id maximum */
#define CMD_ID_MAX 100

/* Command priority minimum */
#define CMD_PRIO_MIN 0
/* Command priority maximum */
#define CMD_PRIO_MAX 255

/* Command data minimum */
#define CMD_DATA_MIN 0
/* Command data maximum */
#define CMD_DATA_MAX INT_MAX

/* Command entry id minimum */
#define CMD_ENTRY_MIN 0
/* Command entry id maximum */
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
	}

end_fclose:
	fclose(input);

end:
	return ret;
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
