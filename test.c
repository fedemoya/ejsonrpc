
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ejsonrpc.h"
#include "frozen.h"

#include "munit.h"


/*****************************
 * Helper data and functions *
 *****************************/


int sum(char *params, char* response, int response_size) {

	struct json_token t;
	int i;

	int a, b, result;
	char aux[10];

    json_scanf_array_elem(params, strlen(params), "", 0, &t);
    strncpy(aux, t.ptr, t.len);
    a = atoi(aux);

    json_scanf_array_elem(params, strlen(params), "", 1, &t);
	strncpy(aux, t.ptr, t.len);
	b = atoi(aux);

	result = a + b;

	struct json_out out = JSON_OUT_BUF(response, response_size);

	return json_printf(&out, "%d", result);
}

int just_error(char *params, char* response, int response_size) {
	return -1;
}

jsonrpc_method_t methods[] = {
		{
			"sum",
			sum
		},
		{
			"just_error",
			just_error
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
	written_bytes = execute_jsonrpc(req, sizeof(req), methods, 2, response, sizeof(response));

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

static MunitResult test_just_error(const MunitParameter params[], void* user_data_or_fixture) {

	/* These are just to silence compiler warnings about the parameters
	 * being unused. */
	(void) params;
	(void) user_data_or_fixture;

	char response[100];
	int written_bytes = 0;

	char req[] = "{\"jsonrpc\":\"2.0\", \"method\":\"just_error\", \"params\":\"anything\", \"id\":87}";
	written_bytes = execute_jsonrpc(req, sizeof(req), methods, 2, response, sizeof(response));

	munit_assert_int(written_bytes, >, 0);

	char *jsonrpc_version, *message;
	int read_bytes, error_code, id;

	read_bytes = json_scanf(response, sizeof(response), "{jsonrpc: %Q, error: {code:%d, message:%Q}, id:%d}", &jsonrpc_version, &error_code, &message, &id);

	munit_assert_int(read_bytes, >, 0);
	munit_assert_int(error_code, ==, INTERNAL_ERROR);

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
  	"/test_just_error", /* name */
	test_just_error, /* test */
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
