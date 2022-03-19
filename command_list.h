#ifndef __COMMAND_LIST_H__
#define __COMMAND_LIST_H__ 1

struct cl;

struct cl *cl_create(void);
int cl_process_insert_lo(int data, struct cl *cl);
int cl_process_insert(int prio, int data, struct cl *cl);
int cl_process_delete(int entry_id, struct cl *cl);
int cl_process_sort(struct cl *cl);
int cl_process_modify(int entry_id, int data, struct cl *cl);
int cl_process_print(struct cl *cl);
int cl_process_reverse(struct cl *cl);
int cl_process_execute(struct cl *cl);
void cl_destroy(struct cl *cl);

#endif
