
CDIALECT = c11

INCLUDES += ./include
LIBS += 
DEFINES += TIGAME_COM_ABI=0

ALL_CFLAGS = $(CFLAGS) -pthread $(foreach include,$(INCLUDES),-I$(include)) $(foreach define,$(DEFINES),-D$(define)) -std=$(CDIALECT)

ifeq ($(BUILD_TYPE),debug)
	ALL_CLFAGS += -g
else ifeq ($(BUILD_TYPE),debug-sanitize)
	ALL_CLFAGS += -g -fsanitize=undefined -fstack-protector-strong
else ifeq ($(BUILD_TYPE),release)
	ALL_CFLAGS += -O3 -fstack-protector-strong
	DEFINES += TIGAME_NO_DEBUG
endif



ALL_LDFLAGS = $(LD_FLAGS) -pthread

OBJECTS := out/Random.o out/Main.o out/Food.o out/Game.o

OUTPUT := tigame

all: $(OUTPUT)


$(OUTPUT): $(OBJECTS)
	$(CC) $(ALL_LDFLAGS) -o $@ $^

out/:
	mkdir out/
	
out/%.o: src/%.c | out/
	$(CC) $(ALL_CFLAGS) -frandom-seed="$@" -o $@ -c $^

clean:
	rm -rf out/
	rm -f $(OUTPUT)
	
rebuild: 
	+make clean
	+make all