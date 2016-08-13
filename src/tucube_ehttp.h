#ifndef _TUCUBE_EHTTP_H
#define _TUCUBE_EHTTP_H

#include "tucube_ehttp_parser.h"
#include "../../tucube_tepoll/src/tucube_tepoll_cldata.h"

struct tucube_ehttp_cldata
{
    int client_socket;
    struct tucube_ehttp_parser* http_parser;
};

int tucube_tepoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list);
int tucube_tepoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args);
int tucube_tepoll_module_clinit(struct tucube_tepoll_cldata_list* cldata_list, int client_socket);
int tucube_tepoll_module_service(struct tucube_module* module, struct tucube_tepoll_cldata*);
int tucube_tepoll_module_cldestroy(struct tucube_tepoll_cldata* cldata);
int tucube_tepoll_module_tldestroy(struct tucube_module* module);
int tucube_tepoll_module_destroy(struct tucube_module* module);

#endif
