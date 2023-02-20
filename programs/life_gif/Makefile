# paths
LIB = ../../lib
INCLUDE = ../../include
MODULES= ../../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files -Werror
CFLAGS = -Wall -Werror -g  -I$(INCLUDE)
LDFLAGS = -lm

# Αρχεία .o
OBJS = life_gif.o $(LIB)/k08.a $(LIB)/libbmp.a
	
# Το εκτελέσιμο πρόγραμμα
EXEC = prog

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Για να φτιάξουμε το k08.a, τρέχουμε το make στο lib directory.
$(LIB)/k08.a:
	$(MAKE) -C $(LIB) k08.a

# το ίδιο για το libbmp.a
$(LIB)/libbmp.a:
	$(MAKE) -C $(LIB) libbmp.a
clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)#inpput   t l  b  r   fr z s d name
	./$(EXEC) rle.txt 0 0 100 100 10 1 1 0 gif_name.gif