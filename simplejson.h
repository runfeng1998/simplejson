#ifndef SIMPLEJSON_H__
#define SIMPLEJSON_H__

#include "assert.h"
#include "string.h"

typedef enum {
    JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY
} json_type;

enum {
    JSON_PARSE_OK = 0,
    JSON_PARSE_EXPECT_VALUE,
    JSON_PARSE_INVALID_VALUE,
    JSON_PARSE_ROOT_NOT_SINGULAR
};

typedef struct {
    json_type type;
} json_value;

typedef struct {
    const char* json;
} json_context;

int json_parse(json_value *value, const char *json);

json_type json_get_type(const json_value* value);

#endif /* SIMPLEJSON_H__ */