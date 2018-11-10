cc = gcc
s = s
c = c
s_source = server_help.c server_daemon.c popen.c server.c
c_source = client_help.c client.c

all: $(s) $(c)

$(s): $(s_source)
	$(cc) -o $(s) $(s_source)

$(c): $(c_source)
	$(cc)  -o $(c) $(c_source)

.PHONY: clean
clean: 
	rm -f s c
