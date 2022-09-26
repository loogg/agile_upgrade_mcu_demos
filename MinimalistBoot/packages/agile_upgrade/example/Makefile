CFLAGS = -I./inc -I../inc -I./tinycrypt-1.1.0/include \
		 -I./fastlz-1.0.1 -I./quicklz-1.0.1

CC = gcc -std=gnu99
OBJDIR = ./build

.PHONY: clean

$(shell mkdir -p $(OBJDIR))

SRC = $(wildcard ./src/*.c) $(wildcard ../src/*.c) $(wildcard ../adapter/*.c) \
	  $(wildcard ./tinycrypt-1.1.0/src/*.c) ./fastlz-1.0.1/fastlz.c ./quicklz-1.0.1/quicklz.c


OBJECTS = $(patsubst %.c,./$(OBJDIR)/%.o,$(notdir $(SRC)))

test : $(OBJECTS)
	${CC} $^ -g -o $@

./$(OBJDIR)/%.o : ./src/%.c
	${CC} -g -c $< -o $@ ${CFLAGS}

./$(OBJDIR)/%.o : ../src/%.c
	${CC} -g -c $< -o $@ ${CFLAGS}

./$(OBJDIR)/%.o : ../adapter/%.c
	${CC} -g -c $< -o $@ ${CFLAGS}

./$(OBJDIR)/%.o : ./tinycrypt-1.1.0/src/%.c
	${CC} -g -c $< -o $@ ${CFLAGS}

./$(OBJDIR)/%.o : ./fastlz-1.0.1/%.c
	${CC} -g -c $< -o $@ ${CFLAGS}

./$(OBJDIR)/%.o : ./quicklz-1.0.1/%.c
	${CC} -g -c $< -o $@ ${CFLAGS}

clean :
	-rm -f ./build/*.o test
