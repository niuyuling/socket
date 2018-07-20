cc = gcc
s = s
c = c
s_source = s_help.c s_daemon.c popen.c s.c
c_source = c_help.c c.c

all: $(s) $(c)

$(s): $(s_source)
	$(cc) -o $(s) $(s_source)

$(c): $(c_source)
	$(cc)  -o $(c) $(c_source)

.PHONY: clean
clean: 
	rm -f s c
