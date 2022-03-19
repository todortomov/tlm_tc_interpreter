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
	int prio = CMD_PRIO_MIN;

	return cl_process_insert(prio, data, cl);
}

int cl_process_insert(int prio, int data, struct cl *cl)
{
	struct node *new;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	new = (struct node *) malloc(sizeof(struct node));
	if (new == NULL) {
		printf("%s: malloc failed.\n", __func__);
		return 1;
	}

	if (idg_get_id_next(cl->idg, &new->entry_id) != 0) {
		printf("%s: entry id generation failed.\n", __func__);
		free(new);
		return 1;
	}
	new->priority = prio;
	new->data = data;
	new->next = NULL;

	if (cl->head == NULL) {
		cl->head = new;
		cl->tail = new;
	} else {
		cl->tail->next = new;
		cl->tail = new;
	}

	return 0;
}

int cl_process_delete(int entry_id, struct cl *cl)
{
	struct node *prev, *curr;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	if (cl->head == NULL) {
		printf("%s: node with entry_id = %d not found, list empty\n",
		       __func__, entry_id);
		return 1;
	}

	if (cl->head == cl->tail) {
		if (cl->head->entry_id == entry_id) {
			free(cl->head);
			cl->head = NULL;
			cl->tail = NULL;
			return 0;
		} else {
			printf("%s: node with entry_id = %d not found.\n",
			       __func__, entry_id);
			return 1;
		}
	}

	curr = cl->head;

	if (curr->entry_id == entry_id) {
		cl->head = curr->next;
		free(curr);
		return 0;
	}

	prev = cl->head;
	curr = cl->head->next;

	while (curr != NULL && curr->entry_id != entry_id) {
		prev = curr;
		curr = curr->next;
	}

	if (curr == NULL) {
		printf("%s: node with entry_id = %d not found.\n",
		       __func__, entry_id);
		return 1;
	}

	if (curr->next == NULL) {
		prev->next = NULL;
		cl->tail = prev;
	} else {
		prev->next = curr->next;
	}

	free(curr);

	return 0;
}

static void list_split(struct node *head, struct node **first, struct node **second)
{
	struct node *fast, *slow;

	if (head == NULL) {
		*first = *second = NULL;
		return;
	}

	if (head->next == NULL) {
		*first = head;
		*second = NULL;
		return;
	}

	slow = head;
	fast = head->next;
	while (fast != NULL) {
		fast = fast->next;
		if (fast != NULL) {
			fast = fast->next;
			slow = slow->next;
		}
	}

	*first = head;
	*second = slow->next;
	slow->next = NULL;
}

static void list_merge(struct node *first, struct node *second, struct node **merged,
		       int (*compar)(struct node *a, struct node *b))
{
	struct node *tail;

	if (first == NULL) {
		*merged = second;
		return;
	}

	if (second == NULL) {
		*merged = first;
		return;
	}

	if (compar(first, second) <= 0) {
		*merged = first;
		first = first->next;
	} else {
		*merged = second;
		second = second->next;
	}

	tail = *merged;

	while (1) {
		if (first == NULL) {
			tail->next = second;
			break;
		} else if (second == NULL) {
			tail->next = first;
			break;
		} else if (compar(first, second) <= 0) {
			tail->next = first;
			tail = tail->next;
			first = first->next;
		} else {
			tail->next = second;
			tail = tail->next;
			second = second->next;
		}
	}
}

static int compare_node(struct node *a, struct node *b)
{
	if (a->priority > b->priority) {
		return -1;
	} else if (a->priority < b->priority) {
		return 1;
	} else if (a->data < b->data) {
		return -1;
	} else if (a->data > b->data) {
		return 1;
	} else {
		return 0;
	}
}

static void list_sort(struct node **head_ref)
{
	struct node *head = *head_ref;
	struct node *first, *second;

	if (head->next == NULL)
		return;

	list_split(head, &first, &second);

	list_sort(&first);
	list_sort(&second);

	list_merge(first, second, head_ref, compare_node);
}

int cl_process_sort(struct cl *cl)
{
	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	if (cl->head == NULL)
		return 0;

	list_sort(&cl->head);

	cl->tail = cl->head;
	while (cl->tail->next != NULL)
		cl->tail = cl->tail->next;

	return 0;
}

int cl_process_modify(int entry_id, int data, struct cl *cl)
{
	struct node *a;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	a = cl->head;
	while (a != NULL && a->entry_id != entry_id)
		a = a->next;

	if (a == NULL) {
		printf("%s: node with entry_id = %d not found.\n",
		       __func__, entry_id);
		return 1;
	}

	a->data = data;

	return 0;
}

int cl_process_print(struct cl *cl)
{
	struct node *a;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	a = cl->head;
	while (a != NULL) {
		printf("(%d, %d, %d)\n", a->entry_id, a->priority, a->data);
		a = a->next;
	}

	return 0;
}

int cl_process_reverse(struct cl *cl)
{
	struct node *first, *second, *third;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	if (cl->head == NULL || cl->head->next == NULL)
		return 0;

	first = cl->head;
	second = first->next;
	third = second->next;

	cl->tail = first;
	first->next = NULL;
	second->next = first;

	while (third != NULL) {
		first = second;
		second = third;
		third = third->next;

		second->next = first;
	}

	cl->head = second;

	return 0;
}

int cl_process_execute(struct cl *cl)
{
	struct node *a;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return 1;
	}

	while (cl->head != NULL) {
		/* execute cl->head here */
		a = cl->head;
		cl->head = cl->head->next;
		free(a);
	}
	cl->tail = NULL;

	return 0;
}

void cl_destroy(struct cl *cl)
{
	struct node *a;

	if (cl == NULL) {
		printf("%s: cl == NULL\n", __func__);
		return;
	}

	idg_destroy(cl->idg);

	while (cl->head != NULL) {
		a = cl->head;
		cl->head = cl->head->next;
		free(a);
	}

	free(cl);
}
