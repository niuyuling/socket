cc = gcc
s = s
c = c
ERROR = -g -Wall -Werror
CFLAGS = -I../iniparser/src -L ../iniparser
LIBS = -liniparser
s_source = server_help.c server_daemon.c popen.c server.c conf.c
SOBJS = $(s_source:.c=.o)
c_source = client_help.c client.c
COBJS = $(c_source:.c=.o)

all: $(s) $(c)

$(s): s.o
	$(cc) $(ERROR) $(CFLAGS) $(SOBJS) -o $(s) $(LIBS)
s.o:
	$(cc) $(ERROR) $(CFLAGS) -c $(s_source)

$(c): c.o
	$(cc) $(ERROR) $(COBJS) -o $(c)
c.o:
	$(cc) $(ERROR) -c $(c_source)

.PHONY: clean
clean:
	rm -rf *.o
	rm -f s c
