cc = gcc
s = s
c = c
cflags = -I../iniparser/src -L ../iniparser
libs = -liniparser
s_source = server_help.c server_daemon.c popen.c server.c
c_source = client_help.c client.c

all: $(s) $(c)

$(s): $(s_source)
	$(cc) $(cflags) -o $(s) $(s_source) $(libs)

$(c): $(c_source)
	$(cc)  -o $(c) $(c_source)

.PHONY: clean
clean: 
	rm -f s c
