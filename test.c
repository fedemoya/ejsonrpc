
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "ejsonrpc.h"

#include "munit.h"


/*****************************
 * Helper data and functions *
 *****************************/


int sum_response_printer(struct json_out *out, va_list *ap) {
    int *method_response = va_arg(*ap, int *);
    return json_printf(out, "%d", *method_response);
}

int sum(char *params, void **method_response, json_printf_callback_t *response_printer) {

    struct json_token t;
    int i;

    int a, b;

    char aux[10];

    json_scanf_array_elem(params, strlen(params), "", 0, &t);
    strncpy(aux, t.ptr, t.len);
    a = atoi(aux);

    json_scanf_array_elem(params, strlen(params), "", 1, &t);
    strncpy(aux, t.ptr, t.len);
    b = atoi(aux);

    int *result = malloc(sizeof(int));
    *result = a + b;

    *method_response = result;
    *response_printer = sum_response_printer;

    return 0;
}

int invalid_params(char *params, void **method_response, json_printf_callback_t *response_printer) {
    return INVALID_PARAMS;
}

jsonrpc_method_t methods[] = {
    {
        "sum",
        sum
    },
    {
        "invalid_params",
        invalid_params
    }
};

/*****************************
 * Unit tests                *
 *****************************/


static MunitResult test_sum(const MunitParameter params[], void* user_data_or_fixture) {


	/* These are just to silence compiler warnings about the parameters
	* being unused. */
	(void) params;
	(void) user_data_or_fixture;

	char response[100];
	int written_bytes = 0;

	char req[] = "{\"jsonrpc\":\"2.0\", \"method\":\"sum\", \"params\":[1,2], \"id\":45}";
	written_bytes = execute_jsonrpc(req, sizeof(req), methods, sizeof(methods)/sizeof(jsonrpc_method_t), response, sizeof(response));

	munit_assert_int(written_bytes, >, 0);

	char *jsonrpc_version;
	int read_bytes, result, id;

	read_bytes = json_scanf(response, sizeof(response), "{jsonrpc: %Q, result: %d, id:%d}", &jsonrpc_version, &result, &id);

	munit_assert_int(read_bytes, >, 0);
	munit_assert_string_equal(jsonrpc_version, "2.0");
	munit_assert_int(result, ==, 3);
	munit_assert_int(id, ==, 45);

	return MUNIT_OK;
}

static MunitResult test_method_not_found(const MunitParameter params[], void* user_data_or_fixture) {

  /* These are just to silence compiler warnings about the parameters
   * being unused. */
  (void) params;
  (void) user_data_or_fixture;

  char response[100];
  int written_bytes = 0;

  char req[] = "{\"jsonrpc\":\"2.0\", \"method\":\"a_non_existent_method\", \"params\":\"anything\", \"id\":87}";
  written_bytes = execute_jsonrpc(req, sizeof(req), methods, sizeof(methods)/sizeof(jsonrpc_method_t), response, sizeof(response));

  munit_assert_int(written_bytes, >, 0);

  char *jsonrpc_version, *message;
  int read_bytes, error_code, id;

  read_bytes = json_scanf(response, sizeof(response), "{jsonrpc: %Q, error: {code:%d, message:%Q}, id:%d}", &jsonrpc_version, &error_code, &message, &id);

  munit_assert_int(read_bytes, >, 0);
  munit_assert_string_equal(jsonrpc_version, "2.0");
  munit_assert_int(error_code, ==, METHOD_NOT_FOUND);
  munit_assert_int(id, ==, 87);

  return MUNIT_OK;
}

static MunitResult test_invalid_params(const MunitParameter params[], void* user_data_or_fixture) {

    /* These are just to silence compiler warnings about the parameters
     * being unused. */
    (void) params;
    (void) user_data_or_fixture;

    char response[100];
    int written_bytes = 0;

    char req[] = "{\"jsonrpc\":\"2.0\", \"method\":\"invalid_params\", \"params\":\"anything\", \"id\":87}";
    written_bytes = execute_jsonrpc(req, sizeof(req), methods, sizeof(methods)/sizeof(jsonrpc_method_t), response, sizeof(response));

    munit_assert_int(written_bytes, >, 0);

    char *jsonrpc_version, *message;
    int read_bytes, error_code, id;

    read_bytes = json_scanf(response, sizeof(response), "{jsonrpc: %Q, error: {code:%d, message:%Q}, id:%d}", &jsonrpc_version, &error_code, &message, &id);

    munit_assert_int(read_bytes, >, 0);
    munit_assert_string_equal(jsonrpc_version, "2.0");
    munit_assert_int(error_code, ==, INVALID_PARAMS);
    munit_assert_int(id, ==, 87);

    return MUNIT_OK;
}

/*****************************
 * Set up test suite         *
 *****************************/


static MunitTest tests[] = {
  {
	"/test_sum", /* name */
	test_sum, /* test */
	NULL, /* setup */
	NULL, /* tear_down */
	MUNIT_TEST_OPTION_NONE, /* options */
	NULL /* parameters */
  },
  {
    "/test_invalid_params", /* name */
    test_invalid_params, /* test */
    NULL, /* setup */
    NULL, /* tear_down */
    MUNIT_TEST_OPTION_NONE, /* options */
    NULL /* parameters */
  },
  {
    "/test_method_not_found", /* name */
    test_method_not_found, /* test */
    NULL, /* setup */
    NULL, /* tear_down */
    MUNIT_TEST_OPTION_NONE, /* options */
    NULL /* parameters */
  },
  /* Mark the end of the array with an entry where the test
   * function is NULL */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "/ejsonrpc_tests", /* name */
  tests, /* tests */
  NULL, /* suites */
  1, /* iterations */
  MUNIT_SUITE_OPTION_NONE /* options */
};


/*****************************
 * Run tests                 *
 *****************************/


int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)])
{

	munit_suite_main(&suite, NULL, argc, argv);

    return 0;

}
