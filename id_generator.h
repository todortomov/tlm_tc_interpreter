#ifndef __ID_GENERATOR_H__
#define __ID_GENERATOR_H__ 1

struct idg;

struct idg *idg_create(void);
int idg_initialize(struct idg *idg);
int idg_get_id_next(struct idg *idg, int *id);
void idg_destroy(struct idg *idg);

#endif
