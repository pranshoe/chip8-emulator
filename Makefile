CC = gcc
EMCC = emcc
COREDIR = src
INCDIR = src/headers
DESKDIR = frontend/desktop
EMDIR = frontend/web

OUTDIR = out
OBJDIR = $(OUTDIR)/objects

CORE_SRCS = $(wildcard $(COREDIR)/*.c)
DESK_SRCS = $(wildcard $(DESKDIR)/*.c)

CORE_OBJS = $(CORE_SRCS:$(COREDIR)/%.c=$(OBJDIR)/%.o)
DESK_OBJS = $(DESK_SRCS:$(DESKDIR)/%.c=$(OBJDIR)/%.o)
WEB_OBJS = $(CORE_SRCS:$(COREDIR)/%.c=$(OBJDIR)/web_%.o)
WEB_OBJS += $(OBJDIR)/chip8_web.o

DESK_BIN = $(OUTDIR)/chip8_desktop
EM_BIN = $(EMDIR)/chip8.js

CFLAGS = -lm
# CFLAGS += -DDEBUG
DESKFLAGS = -lSDL2
EMFLAGS = -s WASM=1 -O3 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 \
          -s EXPORTED_RUNTIME_METHODS=HEAPU8 \
          -s EXPORTED_FUNCTIONS="['_malloc', '_free']"

all: clean

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OBJDIR)/%.o: $(COREDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(DESKDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/web_%.o: $(COREDIR)/%.c | $(OBJDIR)
	$(EMCC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/chip8_web.o : $(EMDIR)/chip8_web.c | $(OBJDIR)
	$(EMCC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR) >/dev/null 2>/dev/null || true

$(DESK_BIN): $(CORE_OBJS) $(DESK_OBJS) | $(OBJDIR)
	$(CC) $(DESKFLAGS) $(CFLAGS) $^ -o $@

desktop: $(DESK_BIN)
	$(DESK_BIN) $(ARGS)

web: $(WEB_OBJS) | $(OBJDIR)
	$(EMCC) $(EMFLAGS) $^ -o $(EM_BIN)
