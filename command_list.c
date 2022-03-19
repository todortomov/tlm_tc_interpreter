#include <stdio.h>
#include <stdlib.h>

#include "telecommand_limits.h"
#include "id_generator.h"

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
	struct idg *idg;
};

struct cl *cl_create(void)
{
	struct cl *cl;

	cl = (struct cl *) malloc(sizeof(struct cl));
	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		goto end_err;
	}

	cl->head = NULL;
	cl->tail = NULL;

	cl->idg = idg_create();
	if (cl->idg == NULL) {
		printf("%s: id generator creation failed.\n", __func__);
		goto end_free;
	}

	if (idg_initialize(cl->idg) != 0) {
		printf("%s: id generator initialization failed.\n", __func__);
		goto end_destroy;
	}

	return cl;

end_destroy:
	idg_destroy(cl->idg);
end_free:
	free(cl);
end_err:
	return NULL;
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

	idg_destroy(cl->idg);

	free(cl);
}
