CUSTOM_FLAGS += -Wall

all: tsl

tsl: tsl.c nxjson/nxjson.c
	gcc $(CUSTOM_FLAGS) -o tsl tsl.c nxjson/nxjson.c

clean:
	rm tsl
