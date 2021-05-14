

/**
   CFLAGS="-I$(realpath ../../include) -I$(realpath ../../build/generated)"
   LDFLAGS="-L$(realpath ../../build/lib) -lcouchbase -Wl,-rpath=$(realpath ../../build/lib)"
   make query

   The example assumes the existence of test bucket and a primary index
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<json-c/json.h>

#include <libcouchbase/couchbase.h>

#include "queries.h"

static void check(lcb_STATUS err, const char *msg)
{
    if (err != LCB_SUCCESS) {
        fprintf(stderr, "[\x1b[31mERROR\x1b[0m] %s: %s\n", msg, lcb_strerror_short(err));
        exit(EXIT_FAILURE);
    }
}

static int err2color(lcb_STATUS err)
{
    switch (err) {
        case LCB_SUCCESS:
            return 49;
        default:
            return 31;
    }
}

static void ln2space(const void *buf, size_t nbuf)
{
    size_t ii;
    char *str = (char *)buf;
    for (ii = 0; ii < nbuf; ii++) {
        if (str[ii] == '\n') {
            str[ii] = ' ';
        }
    }
}


static void txid_callback(lcb_INSTANCE *instance, int type, const lcb_RESPQUERY *resp) {
    struct json_object *parsed_json;
    struct json_object *results;
    struct json_object *txid_obj;
    const char *transaction_id; 
    char *temp;
   

   lcb_respquery_cookie(resp, (void **)&transaction_id);

   check(lcb_respquery_status(resp),"check response status");
    const char *row;
    size_t nrow;
   lcb_respquery_row(resp, &row, &nrow);
   if (!lcb_respquery_is_final(resp)) {
        parsed_json = json_tokener_parse(row);
        json_object_object_get_ex(parsed_json, "txid", &txid_obj);
        temp = json_object_get_string(txid_obj);
        strcpy(transaction_id,temp);
    }
    
}

static void row_callback(lcb_INSTANCE *instance, int type, const lcb_RESPQUERY *resp) {
    const char *row;
    size_t nrow;
    lcb_STATUS rc = lcb_respquery_status(resp);
 
    lcb_respquery_row(resp, &row, &nrow);
    ln2space(row, nrow);
    fprintf(stderr, "[\x1b[%dmQUERY\x1b[0m] %s, (%d) %.*s\n", err2color(rc), lcb_strerror_short(rc), (int)nrow,
            (int)nrow, row);
    if (lcb_respquery_is_final(resp)) {
        fprintf(stderr, "\n");
    }
}
 

// Open a bucket 
static void open_callback(lcb_INSTANCE *instance, lcb_STATUS rc)
{
    printf("open bucket: %s\n", lcb_strerror_short(rc));
}

int main(int argc, char *argv[])
{
lcb_STATUS err; /* return code, that have to be checked */
lcb_INSTANCE *instance;
char *bucket= NULL;
size_t ii;
char *transaction_id = (char *)malloc(64 * sizeof(char));



if (argc < 2) {
        printf("Usage: %s couchbase://host/bucket [ password [ username ] ]\n", argv[0]);
        exit(EXIT_FAILURE);
    }


{
    // Initialize the cluster
        lcb_CREATEOPTS *create_options = NULL;
        lcb_createopts_create(&create_options, LCB_TYPE_CLUSTER);
        
        lcb_createopts_connstr(create_options, argv[1], strlen(argv[1]));
        lcb_createopts_credentials(create_options, argv[2], strlen(argv[2]), argv[3], strlen(argv[3]));

        check(lcb_create(&instance, create_options), "create couchbase handle");
        lcb_createopts_destroy(create_options);

        check(lcb_connect(instance), "schedule connection");
        lcb_wait(instance, LCB_WAIT_DEFAULT);
        //check(lcb_get_bootstrap_status(instance), "bootstrap from cluster");
        check(lcb_cntl(instance, LCB_CNTL_GET, LCB_CNTL_BUCKETNAME, &bucket), "get bucket name");
        printf("%s\n",bucket);
        if (strcmp(bucket, "test") != 0) {
            printf("expected bucket to be \"test\"");
        }
    }


        
        
    for (ii = 0; ii < num_queries; ii++) {
        lcb_CMDQUERY *cmd;        
        lcb_cmdquery_create(&cmd);

        check(lcb_cmdquery_statement(cmd, queries[ii].query, strlen(queries[ii].query)), "set QUERY statement");   

        printf("----> \x1b[1m%s\x1b[0m\n", queries[ii].query);

        if (ii == 0) {
            lcb_cmdquery_callback(cmd, txid_callback);
            lcb_wait(instance, LCB_WAIT_DEFAULT);
            check(lcb_query(instance, transaction_id, cmd), "schedule QUERY operation");
            lcb_wait(instance, LCB_WAIT_DEFAULT);            
       } else {
            char buf[100];
            sprintf(buf,"\"%s\"",transaction_id);
            lcb_cmdquery_callback(cmd, row_callback);
           // SET rest option pretty to true and txtimeout to 3s
            check(lcb_cmdquery_option(cmd, "pretty", strlen("pretty"), "true", strlen("true")),"set QUERY 'pretty' option");
            check(lcb_cmdquery_option(cmd, "txtimeout", strlen("txtimeout"), "\"3s\"", strlen("\"3s\"")),"set QUERY 'txtimeout' option");
            check(lcb_cmdquery_option(cmd, "txid", strlen("txid"),buf, strlen(buf)),"set QUERY 'txtimeout' option");
            check(lcb_query(instance, NULL, cmd), "schedule QUERY operation");
            lcb_wait(instance, LCB_WAIT_DEFAULT);

       }        

        lcb_cmdquery_destroy(cmd);
        lcb_wait(instance, LCB_WAIT_DEFAULT);
    }

    /* Now that we're all done, close down the connection handle */
    lcb_destroy(instance);
    return 0;
}
