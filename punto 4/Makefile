.PHONY: all clean

SRC = read_root.c read_file.c recovery_file.c
BIN = $(SRC:.c=)

clean:
	rm -f $(BIN) $(OBJ)

all: 
	gcc read_root.c -o read_root
	gcc read_file.c -o read_file
	gcc recovery_file.c -o recovery_file