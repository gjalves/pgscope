CFLAGS=-g -Wall -Ilibpg_query -D_GNU_SOURCE

LDFLAGS=-Llibpg_query -lyajl -lpg_query

all: pgscope

pgscope: pgscope.c
	$(CC) $(CFLAGS) -o pgscope pgscope.c $(LDFLAGS)
