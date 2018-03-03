
#include "frozen.h"

/*
 * JSONRPC examples
 *
 * --> request
 * <-- response
 *
 * Success examples
 *
 * --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
 * <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
 *
 * --> {"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3}
 * <-- {"jsonrpc": "2.0", "result": 19, "id": 3}
 *
 *
 * Error examples
 *
 * --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
 * <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
 *
 */

#define PARSE_ERROR -32700 // Invalid JSON was received.
#define INVALID_REQUEST -32600 // The JSON sent is not a valid Request object.
#define	METHOD_NOT_FOUND -32601 // Method not found. The method does not exist / is not available.
#define INVALID_PARAMS -32602 // Invalid params. Invalid method parameter(s).
#define INTERNAL_ERROR -32603  // Internal error. Internal JSON-RPC error.

typedef struct {
	char *method_name;
	/*
	 * Input:
	 *  - params
	 * Output
	 * - method_response
	 * - response_printer
	 */
	int (*method)(char *params, void **method_response, json_printf_callback_t *response_printer);
} jsonrpc_method_t;

int execute_jsonrpc(const char *jsonrpc_req, int request_size, jsonrpc_method_t *methods, int methods_size, char *jsonrpc_res, int response_size);
