OUTPUT = raytracer
OUTPUTFOLDERS = bin obj
SHARED = list vector objects xmlio raytrace avsio
SOURCES = $(OUTPUT) $(SHARED)

SHAREDOBJS = $(patsubst %,obj/%.o,$(SHARED))
OBJECTS = $(patsubst %,obj/%.o,$(SOURCES))
HEADERS = $(patsubst %,include/%.h,$(SHARED))
BINARY = $(patsubst %,bin/%,$(OUTPUT))

CC = gcc
INCLUDEDIR = -Iinclude
CFLAGS= $(INCLUDEDIR) -I/usr/include/libxml2
LDLIBS= -lm
LDFLAGS = -L/usr/lib -lxml2

$(OUTPUT): % : bin/%

all: $(OUTPUTFOLDERS) imgs $(OUTPUT)

imgs:
	mkdir resources/imgs

$(OUTPUTFOLDERS):
	mkdir $@

$(BINARY): bin/%: obj/%.o $(SHAREDOBJS)
	$(CC) -o $@ $(SHAREDOBJS) $< $(LDLIBS) $(CFLAGS) $(LDFLAGS)
	
$(OBJECTS): obj/%.o : src/%.c $(HEADERS)
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY : clean
clean:
	rm -f obj/*.o bin/* resources/imgs/*
	
.PHONY : cleanall
cleanall:
	rm -f obj/*.o bin/* resources/imgs/*
	rmdir obj bin resources/imgs
