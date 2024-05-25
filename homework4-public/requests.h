#include "json.hpp"

using json = nlohmann::json;

#ifndef _REQUESTS_
#define _REQUESTS_

char* compute_delete_request(const char *host, const char *url, char* token);

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(const char *host, const char *url, char* cookie, char* token);
// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(const char *host, const char *url, const char* content_type, char* token, json data);
#endif
