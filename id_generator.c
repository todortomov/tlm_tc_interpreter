#include <stdlib.h>

#include "telecommand_limits.h"

#include "id_generator.h"

#define IDG_ID_MIN CMD_ENTRY_MIN
#define IDG_ID_MAX CMD_ENTRY_MAX

struct idg {
	int id_current;
};

struct idg *idg_create(void)
{
	struct idg *idg;

	idg = (struct idg *) malloc(sizeof(struct idg));

	return idg;
}

int idg_initialize(struct idg *idg)
{
	if (idg == NULL)
		return 1;

	idg->id_current = IDG_ID_MIN - 1;

	return 0;
}

int idg_get_id_next(struct idg *idg, int *id)
{
	if (idg == NULL || id == NULL)
		return 1;

	if (idg->id_current == IDG_ID_MAX)
		return 1;

	*id = ++idg->id_current;

	return 0;
}

void idg_destroy(struct idg *idg)
{
	if (idg != NULL)
		free(idg);
}
