#include "bte.h"

/*******************************************************************************
 * headers
 */

#include <stddef.h>

#include "data_types.h"

#include "gki.h"

/*******************************************************************************
 * types
 */

// I have no idea how accurate this is
typedef struct load
{
	struct load *next;
	UINT32 size;
} tBT_LOAD;

/*******************************************************************************
 * variables
 */

tBT_LOAD bt_load_tbl;

/*******************************************************************************
 * functions
 */

void BTE_LoadStack(void)
{
	tBT_LOAD *load = &bt_load_tbl;

	for (load = &bt_load_tbl; load->next; ++load)
		load->next->next = GKI_os_malloc(load->size);
}

void BTE_UnloadStack(void)
{
	tBT_LOAD *load = &bt_load_tbl;

	for (load = &bt_load_tbl; load->next; ++load)
	{
		if (load->next->next)
		{
			GKI_os_free(load->next->next);
			load->next->next = NULL;
		}
	}
}
