#include <stdio.h>

char help(void)
{
    static const char my_name[] = "s";
    static const char version[] = "version 1.0";
    static const char subject[] = "s";

    static const struct {
    const char *f, *s, *e;
    } author = {
    "aixiao@aixiao.me", "aixiao", "Author:",};
    static const char usage[] = "usage: s [-d] [-s stop|status] [-p pid file] | [-?h]";

    static const char *s_help[] = {
    "",
    "    -?,-h     : this help",
    "    -d        : daemon",
    "    -s        : signal: stop,status",
    "    -p,       : pid file(default: s.pid)",
    "",
    "",
    0
    };


    fprintf(stderr, "%s %s, %s\n", my_name, version, subject);
    fprintf(stderr, "%s %s %s\n", author.e, author.s, author.f);
    fprintf(stderr, "%s %s\n", my_name, usage);

    int l;
    for (l = 0; s_help[l]; l++)
        fprintf(stderr, "%s\n", s_help[l]);
}
