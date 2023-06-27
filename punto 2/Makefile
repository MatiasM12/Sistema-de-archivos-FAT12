.PHONY: all clean

SRC = read_mbr.c read_first_partition.c read_boot.c
BIN = $(SRC:.c=)

clean:
	rm -f $(BIN) $(OBJ)

all: 
	gcc read_first_partition.c -o read_first_partition
	gcc read_mbr.c -o  read_mbr
	gcc read_boot.c -o  read_boot
