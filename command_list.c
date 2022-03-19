#include <stdio.h>
#include <stdlib.h>

#include "telecommand_limits.h"

#include "command_list.h"

struct node {
        int entry_id;
        int priority;
        int data;
        struct node *next;
};

struct cl {
	struct node *head;
	struct node *tail;
};

struct cl *cl_create(void)
{
	struct cl *cl;

	cl = (struct cl *) malloc(sizeof(struct cl));
	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return NULL;
	}

	cl->head = NULL;
	cl->tail = NULL;

	return cl;
}

int cl_process_insert_lo(int data, struct cl *cl)
{
	return 0;
}

int cl_process_insert(int prio, int data, struct cl *cl)
{
	return 0;
}

int cl_process_delete(int entry_id, struct cl *cl)
{
	return 0;
}

int cl_process_sort(struct cl *cl)
{
	return 0;
}

int cl_process_modify(int entry_id, int data, struct cl *cl)
{
	return 0;
}

int cl_process_print(struct cl *cl)
{
	return 0;
}

int cl_process_reverse(struct cl *cl)
{
	return 0;
}

int cl_process_execute(struct cl *cl)
{
	return 0;
}

void cl_destroy(struct cl *cl)
{
	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return;
	}

	free(cl);
}
