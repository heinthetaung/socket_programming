/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include "db.h"

int save_schedule(char *json) {
    db_insert_json(json, "Schedule");
}

int delete_schedule(char *id) {
    db_delete_json(id);
}

int save_whitelist(char * json) {
    db_insert_json(json, "Whitelist");
}

int delete_whitelist(char *id) {
    db_delete_json(id);
}

int get_schedule(char * result[], int *size) {
    db_query_json("Description", "Schedule", result, size);
}

int get_pin_from_whitelist(char * tagid, char * result[], int *size) {
    db_query_json("TAGID", tagid, result, size);
}

int db_query_json(char * key, char * val, char * result[] , int * size) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_t *query;
    char *str;
    int count = 0;
    mongoc_init();

    client = mongoc_client_new(
            "mongodb://localhost:27017/?appname=find-specific-example");
    collection = mongoc_client_get_collection(client, DB_NAME, COLL_NAME);
    query = bson_new();
    BSON_APPEND_UTF8(query, key, val);

    cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);

    while (mongoc_cursor_next(cursor, &doc)) {
        str = bson_as_canonical_extended_json(doc, NULL);
//        printf("%s\n", str);
        result[count] = str;
        count +=1;
//        bson_free(str);
    }
    *size = count;
//    printf("Size: %d\n", *size);
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return 0;
}

int db_insert_json(char * json, char * description) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_oid_t oid;
    bson_t *doc;


    doc = bson_new_from_json((const uint8_t *) json, -1, &error);

    mongoc_init();

    client = mongoc_client_new("mongodb://localhost:27017/?appname=insert-example");
    collection = mongoc_client_get_collection(client, DB_NAME, COLL_NAME);

    //    doc = bson_new();
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(doc, "_id", &oid);
    if (description != NULL) {
        BSON_APPEND_UTF8(doc, "Description", description);
    }


    char * print_string = bson_as_json(doc, NULL);
    printf("%s\n", print_string);
    bson_free(print_string);
    if (!mongoc_collection_insert_one(
            collection, doc, NULL, NULL, &error)) {
        fprintf(stderr, "%s\n", error.message);
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return 0;
}

int db_delete_json(char * id) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_oid_t oid;
    bson_t *doc;

    mongoc_init();

    client = mongoc_client_new("mongodb://localhost:27017/?appname=delete-example");
    collection = mongoc_client_get_collection(client, DB_NAME, COLL_NAME);

    //insert
    //   doc = bson_new ();
    //   bson_oid_init (&oid, NULL);
    //   BSON_APPEND_OID (doc, "_id", &oid);
    //   BSON_APPEND_UTF8 (doc, "hello", "world");

    //   char * string  = bson_as_json(doc, NULL);
    //   printf("%s\n", string);
    //   if (!mongoc_collection_insert_one (collection, doc, NULL, NULL, &error)) {
    //      fprintf (stderr, "Insert failed: %s\n", error.message);
    //   }

    //   bson_destroy (doc);

    //delete
    doc = bson_new();
    BSON_APPEND_UTF8(doc, "ID", id);

    if (!mongoc_collection_delete_one(
            collection, doc, NULL, NULL, &error)) {
        fprintf(stderr, "Delete failed: %s\n", error.message);
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return 0;
}