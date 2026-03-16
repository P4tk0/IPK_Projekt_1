CC=gcc
CFLAGS=-Wall -Wextra -std=c17

TARGET=ipk-L4-scan

SRC=$(wildcard src/*.c)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

NixDevShellName:
	@echo c

clean:
	rm -f $(TARGET)

test:
	./tests/test_basic.sh

env:
	@echo "Entering test enviroment"
	nix develop "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#c"
