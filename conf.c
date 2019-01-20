#include "conf.h"

void read_conf(char *file, conf * p)
{
    dictionary *ini = iniparser_load(file);

    // server module
    p->len_ip = strlen(iniparser_getstring(ini, "server:IP", NULL)) + 1;
    p->ip = (char *)malloc(p->len_ip);
    if (p->ip == NULL) {
        free(p->ip);
    }
    memset(p->ip, 0, p->len_ip);
    memcpy(p->ip, iniparser_getstring(ini, "server:IP", NULL), p->len_ip);
    
    p->port = iniparser_getint(ini, "server:PORT", 0);
    
    p->len_clientip = strlen(iniparser_getstring(ini, "server:CLIENTIP", NULL)) + 1;
    p->clientip = (char *)malloc(p->len_clientip);
    if (p->clientip == NULL) {
        free(p->clientip);
    }
    memset(p->clientip, 0, p->len_clientip);
    memcpy(p->clientip, iniparser_getstring(ini, "server:CLIENTIP", NULL), p->len_clientip);
    
    p->len_clientip1 = strlen(iniparser_getstring(ini, "server:CLIENTIP1", NULL)) + 1;
    p->clientip1 = (char *)malloc(p->len_clientip1);
    if (p->clientip1 == NULL) {
        free(p->clientip1);
    }
    memset(p->clientip1, 0, p->len_clientip1);
    memcpy(p->clientip1, iniparser_getstring(ini, "server:CLIENTIP1", NULL), p->len_clientip1);
    
    p->len_pid_file = strlen(iniparser_getstring(ini, "server:PID_FILE", NULL)) + 1;
    p->pid_file = (char *)malloc(p->len_pid_file);
    if (p->pid_file == NULL) {
        free(p->pid_file);
    }
    memset(p->pid_file, 0, p->len_pid_file);
    memcpy(p->pid_file, iniparser_getstring(ini, "server:PID_FILE", NULL), p->len_pid_file);
    
    iniparser_freedict(ini);
}

void free_conf(conf *p)
{
    free(p->ip);
    free(p->clientip);
    free(p->clientip1);
    free(p->pid_file);
}