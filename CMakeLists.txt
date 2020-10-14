PROJECT(simplejson)

add_library(simplejson simplejson.c)
add_executable(simplejson_test test.c)

target_link_libraries(simplejson_test simplejson)

