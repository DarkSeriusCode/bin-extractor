CC     = clang
CFLAGS = -Wall -Wextra

PROJECT_BIN    = bin/bin-extractor
INSTALL_PREFIX = /usr/local

ifdef release
	CFLAGS += -O3
else
	CFLAGS += -O0 -g
endif

SRCS    = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJS    = $(patsubst src/%.c,obj/%.o,$(SRCS))

$(shell mkdir -p obj bin)

.PHONY: all
all: $(PROJECT_BIN)

$(PROJECT_BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROJECT_BIN) $(OBJS)

obj/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf obj bin out test

.PHONY: install
install: $(PROJECT_BIN)
	install -m 775 $< $(INSTALL_PREFIX)/bin

.PHONY: uninstall
uninstall:
	rm $(INSTALL_PREFIX)/$(PROJECT_BIN)
