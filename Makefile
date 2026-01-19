MODULES = pg_strict
EXTENSION = pg_strict
DATA = pg_strict--1.0.sql
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)