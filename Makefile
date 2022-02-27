RAYLIB_PATH = raylib-4.0.0/src/
LIB_OBJS=rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o
NOGL_LIB_OBJS=utils.o raudio.o
LIB_OBJS_PATH=$(addprefix $(RAYLIB_PATH), $(LIB_OBJS))
NOGL_LIB_OBJS_PATH=$(addprefix $(RAYLIB_PATH), $(NOGL_LIB_OBJS))
CFLAGS=-Os -Wall -DPLATFORM_WEB
CC=docker run --rm -v $(shell pwd):/src -u $(shell id -u):$(shell id -g) emscripten/emsdk:3.1.1 emcc
EMAR=docker run --rm -v $(shell pwd):/src -u $(shell id -u):$(shell id -g) emscripten/emsdk:3.1.1 emar

wa_lib: $(LIB_OBJS_PATH)
	$(EMAR) rcs lib/libraylib.a $(LIB_OBJS_PATH)

wasm: wa_lib
	$(CC) cpp/main.cpp -o build/breakout.html -Wall ./lib/libraylib.a -I. -I./include/ -L. -L./lib/ -s USE_GLFW=3 -s TOTAL_MEMORY=134217728 -s FORCE_FILESYSTEM=1 --preload-file assets -DPLATFORM_WEB

%.o:%.c
	$(CC) -c $^ $(CFLAGS) $(if $(filter $@, $(NOGL_LIB_OBJS_PATH)), , -DGRAPHICS_API_OPENGL_ES2) -o $@

clean:
	rm build/*
	rm $(RAYLIB_PATH)*.o
	rm lib/*
