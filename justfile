default: run

# Compiler flags
CC := "clang"
CFLAGS := "-Wall -Wextra -g"
LDFLAGS := "-lraylib -lm -lpthread -ldl -lrt -lX11"

build:
    {{CC}} {{CFLAGS}} main.c -o main {{LDFLAGS}}

run: build
    ./main

clean:
    rm -f main

release:
    {{CC}} -Wall -Wextra -O2 main.c -o main {{LDFLAGS}}
