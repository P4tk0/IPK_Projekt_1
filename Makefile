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

env:
	@echo "Entering test enviroment"
	nix develop "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#c"

test:
ifdef TEST
	@case "$(TEST)" in \
		args) ./tests/arg_parser_tests.sh ;; \
		tcp)  ./tests/tcp_tests.sh ;; \
		udp)  ./tests/udp_tests.sh ;; \
		all)  ./tests/run_all.sh ;; \
		*)    echo "Unknown test '$(TEST)'"; echo "Available: args, tcp, udp, all" ;; \
	esac
else
	@echo "Usage: make test TEST={TEST}"
	@echo ""
	@echo "Available tests:"
	@echo "  args  - argument parser tests"
	@echo "  tcp   - TCP scanning tests"
	@echo "  udp   - UDP scanning tests"
	@echo "  all   - run all tests with aggregated statistics"
endif

.PHONY: test NixDevShellName env clean