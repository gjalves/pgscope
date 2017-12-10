#include <pg_query.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ftw.h>
#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>
#include <yajl/yajl_gen.h>

struct {
    char *name;
} json_tree[64];

typedef struct {
    int json_tree_level;
} *json_t;

typedef struct {
    FILE *tags;
    char *basepath;
    int basepath_len;
    json_t json;
    const char *current_file;
} *pgscope_t;

pgscope_t pgscope;

int json_search(json_t json, char *args, ...) __attribute__ ((__sentinel__));

int json_search(json_t json, char *args, ...)
{
    va_list ap;
    char *x = args;
    int level = json->json_tree_level;

    if(args == NULL) goto err;
    
    va_start(ap, args);
    do {
        if(x == NULL) goto ok;
        if(level < 2) goto err;
        if(strcmp(x, json_tree[level].name) != 0) goto err;
        level--;
    } while((x = va_arg(ap, char *)));

ok:
    va_end(ap);
    return 0;
err:
    va_end(ap);
    return -1;

}

static int parse_pg_null(void *ctx)
{
    return 1;
}

static int parse_pg_boolean(void *ctx, int boolean)
{
    return 1;
}

static int parse_pg_integer(void *ctx, long long integerVal)
{
    return 1;
}

static int parse_pg_double(void *ctx, double doubleVal)
{
    return 1;
}

static int parse_pg_number(void *ctx, const char * s, size_t l)
{
    return 1;
}

char *defname = NULL;
static int parse_pg_string(void *ctx, const unsigned char *stringVal, size_t stringLen)
{
    pgscope_t pgscope = ctx;

    if(json_search(pgscope->json, "str", "String", "funcname", "CreateFunctionStmt", NULL) == 0) {
        const char *file = pgscope->current_file + pgscope->basepath_len + 1;
        fprintf(pgscope->tags, "%.*s\t%s\t/\\cfunction \\+%.*s/;\" f\n", (int)stringLen, stringVal, file, (int)stringLen, stringVal);

    }

    if(json_search(pgscope->json, "defname", "DefElem", NULL) == 0) {
        if(defname) free(defname);
        defname = strndup((char *)stringVal, stringLen);
    }

/*    if(json_search("str", "String", "arg", "DefElem", "options", "CreateFunctionStmt", NULL) == 0) {
        if(strcmp(defname, "as") == 0) {
            printf("!!! FOUND !!!! %s %.*s\n", defname, (int)stringLen, stringVal);
        }
    }*/

    return 1;
}

static int parse_pg_start_map(void *ctx)
{
    pgscope_t pgscope = ctx;

    pgscope->json->json_tree_level++;
    json_tree[pgscope->json->json_tree_level].name = NULL;
    return 1;
}

static int parse_pg_map_key(void *ctx, const unsigned char *stringVal, size_t stringLen)
{
    pgscope_t pgscope = ctx;

    if(json_tree[pgscope->json->json_tree_level].name) free(json_tree[pgscope->json->json_tree_level].name);
    json_tree[pgscope->json->json_tree_level].name = strndup((char *)stringVal, stringLen);

/*
    int i;
    printf("MAP_KEY ");
    for(i = json_tree_level; i > 0; i--) {
        if(i < json_tree_level) printf(", ");
        printf("\"%s\"", json_tree[i].name);
    }
    printf("\n");*/

    return 1;
}

static int parse_pg_end_map(void *ctx)
{
    pgscope_t pgscope = ctx;

    if(json_tree[pgscope->json->json_tree_level].name) free(json_tree[pgscope->json->json_tree_level].name);
    pgscope->json->json_tree_level--;
    return 1;
}

static int parse_pg_start_array(void *ctx)
{
    return 1;
}

static int parse_pg_end_array(void *ctx)
{
    return 1;
}


static yajl_callbacks callbacks = {
    parse_pg_null,
    parse_pg_boolean,
    parse_pg_integer,
    parse_pg_double,
    parse_pg_number,
    parse_pg_string,
    parse_pg_start_map,
    parse_pg_map_key,
    parse_pg_end_map,
    parse_pg_start_array,
    parse_pg_end_array,
};

int parse_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    char buf[65536];
    FILE *fd;
    long fsize;
    char *ext;
    const char *file;

    pgscope->current_file = fpath;
    file = fpath + pgscope->basepath_len + 1;
    if((ext = rindex(file, '.')) == NULL) return 0;
    if(strcmp(ext, ".sql") != 0) return 0;

    if((fd = fopen(fpath, "r")) == NULL) {
        perror(fpath);
        exit(EXIT_FAILURE);
    }

    fsize = fread(buf, 1, sizeof(buf) - 1, fd);
    buf[fsize] = 0;
  
    PgQueryParseResult result;
  
    result = pg_query_parse(buf);
  
    if (result.error) {
        printf("error: %s at %d\n", result.error->message, result.error->cursorpos);
    } else {
//        printf("%s\n", result.parse_tree);
    }
  
    pgscope->json = malloc(sizeof(*pgscope->json));
    pgscope->json->json_tree_level = 0;
    yajl_status status;
    yajl_handle handler = yajl_alloc(&callbacks, NULL, pgscope);
//yajl_config(handler, yajl_allow_comments, 1);  
//yajl_config(handler, yajl_dont_validate_strings, 1);  
//yajl_config(handler, yajl_allow_trailing_garbage, 1);  
//yajl_config(handler, yajl_allow_multiple_values, 1);  
//yajl_config(handler, yajl_allow_partial_values, 1);  
    status = yajl_parse(handler, (const unsigned char *)result.parse_tree, strlen(result.parse_tree));
    if(status != yajl_status_ok) {
        printf("FAILURE\n");
        exit(EXIT_FAILURE);
    }

    status = yajl_complete_parse(handler);
    if (status != yajl_status_ok) {  
        unsigned char * str = yajl_get_error(handler, 1, (unsigned char *)buf, fsize);
        fprintf(stderr, "%s", (const char *) str);  
        yajl_free_error(handler, str);  
    }


    yajl_free(handler);

    pg_query_free_parse_result(result);
  
 
/*    PgQueryPlpgsqlParseResult pgresult;
  
    pgresult = pg_query_parse_plpgsql(test);
  
    if (pgresult.error) {
        printf("error: %s at %d\n", pgresult.error->message, pgresult.error->cursorpos);
    } else {
        printf("%s\n", pgresult.plpgsql_funcs);
    }
  
    pg_query_free_plpgsql_parse_result(pgresult);
  */
  
  
    return 0;
}

int main(int argc, char **argv)
{
    pgscope = malloc(sizeof(*pgscope));

    pgscope->tags = fopen("tags", "w+");
    fprintf(pgscope->tags, "!_TAG_FILE_FORMAT	2	/extended format; --format=1 will not append ;\" to lines/\n");
    fprintf(pgscope->tags, "!_TAG_FILE_SORTED	0	/0=unsorted, 1=sorted, 2=foldcase/\n");
    fprintf(pgscope->tags, "!_TAG_PROGRAM_AUTHOR	Gustavo Junior Alves	/gjalves@gjalves.com.br/\n");
    fprintf(pgscope->tags, "!_TAG_PROGRAM_NAME	pgscope	//\n");
    fprintf(pgscope->tags, "!_TAG_PROGRAM_URL	https://specchio.info	/official site/\n");
    fprintf(pgscope->tags, "!_TAG_PROGRAM_VERSION	0.0	//\n");

    pgscope->basepath = realpath(".", NULL);
    pgscope->basepath_len = strlen(pgscope->basepath);

    nftw(pgscope->basepath, parse_file, 20, 0);
    fclose(pgscope->tags);
    free(pgscope);
    exit(EXIT_SUCCESS);
}
