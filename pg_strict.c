#include "postgres.h"
#include "fmgr.h"
#include "utils/guc.h"
#include "optimizer/planner.h"
#include "nodes/parsenodes.h"

PG_MODULE_MAGIC;

void _PG_init(void);

static bool pg_strict_enabled = true;
static planner_hook_type prev_planner_hook = NULL;

static PlannedStmt * pg_strict_planner_hook(Query* query, const char* query_string, int cursorOptions, ParamListInfo boundParams) {
  if (pg_strict_enabled) {
    if (query->commandType == CMD_UPDATE || query->commandType == CMD_DELETE) {
      Assert(query->jointree != NULL);
      if (query->jointree->quals == NULL) {
				ereport(ERROR,
						(errcode(ERRCODE_CARDINALITY_VIOLATION),
						 errmsg("DELETE and UPDATE requires a WHERE clause"),
             errhint("To disable this check, set pg_strict.enabled to false.")));
      }
    }
  }

  if (prev_planner_hook)
    return prev_planner_hook(query, query_string, cursorOptions, boundParams);
  else
    return standard_planner(query, query_string, cursorOptions, boundParams);
}

void _PG_init(void) {
  DefineCustomBoolVariable("pg_strict.enabled",
                           "Enable or disable strict mode for DELETE and UPDATE statements.",
                           NULL,
                           &pg_strict_enabled,
                           true,
                           PGC_SUSET,
                           0,
                           NULL,
                           NULL,
                           NULL);

  prev_planner_hook = planner_hook;
  planner_hook = pg_strict_planner_hook;
}