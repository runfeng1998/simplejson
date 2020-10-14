#include "simplejson.h"

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch) ((ch) >='0' && (ch)<='9')
#define ISDIGIT1TO9(ch) ((ch) >='1' && (ch)<='9')

static void json_parse_whitespace(json_context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    c->json = p;
}

static int json_parse_literal(json_context *c, json_value *v, const char *literal, json_type type) {
    size_t len = strlen(literal), i;
    EXPECT(c, literal[0]);
    for (i = 1; i < len; i++) {
        if (c->json[i - 1] != literal[i]) return JSON_PARSE_INVALID_VALUE;
    }
    c->json += len - 1;
    v->type = type;
    return JSON_PARSE_OK;
}

static int json_parse_number(json_context *c, json_value *v) {
    /*
     * number = [ "-" ] int [ frac ] [ exp ]
     * int = "0" / digit1-9 *digit
     * frac = "." 1*digit
     * exp = ("e" / "E") ["-" / "+"] 1*digit
     * 负数 整数 小数 指数
     */
    const char *p = c->json;
    if (*p == '-') p++;

    if (*p == '0') p++;
    else {
        if (!ISDIGIT1TO9(*p)) return JSON_PARSE_INVALID_VALUE;
        p++;
        while (ISDIGIT(*p)) p++;
    }

    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return JSON_PARSE_INVALID_VALUE;
        p++;
        while (ISDIGIT(*p)) p++;
    }

    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return JSON_PARSE_INVALID_VALUE;
        p++;
        while (ISDIGIT(*p)) p++;
    }
    errno = 0;
    char *end;
    v->n = strtod(c->json, &end);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return JSON_PARSE_NUMBER_TOO_BIG;
    v->type = JSON_NUMBER;
    c->json = p;
    /* 这里不用assert是因为
     * 自己解析比strtod严格
     * 所以p<=end
     * 以p为准 不管end*/
//    assert(end == p);
    return JSON_PARSE_OK;
}

/*
static int json_parse_number(json_context *c, json_value *v) {
    char *end;
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return JSON_PARSE_INVALID_VALUE;
    v->type = JSON_NUMBER;
    c->json = end;
    return JSON_PARSE_OK;
}
*/

static int json_parse_value(json_context *c, json_value *v) {
    switch (*c->json) {
        case 'n':
            return json_parse_literal(c, v, "null", JSON_NULL);
        case 't':
            return json_parse_literal(c, v, "true", JSON_TRUE);
        case 'f':
            return json_parse_literal(c, v, "false", JSON_FALSE);
        default:
            return json_parse_number(c, v);
        case '\0':
            return JSON_PARSE_EXPECT_VALUE;
    }
}

int json_parse(json_value *v, const char *json) {
    json_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = JSON_NULL;
    json_parse_whitespace(&c);
    if ((ret = json_parse_value(&c, v)) == JSON_PARSE_OK) {
        json_parse_whitespace(&c);
        if (c.json[0] != '\0') {
            v->type = JSON_NULL;
            ret = JSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

json_type json_get_type(const json_value *v) {
    assert(v != NULL);
    return v->type;
}

double json_get_number(const json_value *v) {
    assert(v != NULL && v->type == JSON_NUMBER);
    return v->n;
}