#include <stdio.h>

char help(void)
{
    static const char my_name[] = "c";
    static const char version[] = "version 1.0";
    static const char subject[] = "c";

    static const struct {
    const char *f, *s, *e;
    } author = {
    "aixiao@aixiao.me", "aixiao", "Author:",};
    static const char usage[] = "usage: c [IP address] & [\"-f file | -k pid\" | \"command\"] | [-?h]";

    static const char *s_help[] = {
    "",
    "    -?,-h     : this help",
    "    -f file   : Transfer file",
    "    -k pid    : Child process PID",
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
