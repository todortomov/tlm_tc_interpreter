#ifndef __TELECOMMAND_LIMITS_H__
#define __TELECOMMAND_LIMITS_H__ 1

#include <limits.h>

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

#endif
