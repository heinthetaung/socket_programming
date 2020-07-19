/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>

int insert_json(char * json) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_oid_t oid;
    bson_t *doc;

    
    doc = bson_new_from_json ((const uint8_t *)json, -1, &error);
    
    mongoc_init();

    client = mongoc_client_new("mongodb://localhost:27017/?appname=insert-example");
    collection = mongoc_client_get_collection(client, "mydb", "mycoll");

//    doc = bson_new();
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(doc, "_id", &oid);
//    BSON_APPEND_UTF8(doc, "hello", "world");

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