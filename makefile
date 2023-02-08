.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o .unittest .profraw .profdata .coverage

CC       = clang
CPROF    = xcrun llvm-profdata
CCOV     = xcrun llvm-cov

SANITIZE = -fsanitize=address -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error
COVERAGE = -fprofile-instr-generate -fcoverage-mapping
OPTS     = $(SANITIZE) $(COVERAGE) -Werror -Weverything -Wno-padded -Wno-poison-system-directories
INCS     = -I. -Isrc

.PHONY: all
all: false_int src/false.coverage

false_int: interpreter.o src/false.o utils/file.o src/stack.o src/slice.o src/storage.o src/token.o
	$(CC) $(OPTS) $(INCS) $^ -o $@

src/false.unittest: src/test_false.o src/stack.o src/slice.o src/storage.o src/token.o

.c.o:
	$(CC) $(OPTS) $(INCS) -c $^ -o $@

.profdata.coverage:
	$(CCOV) show $*.unittest -instr-profile=$< $*.c > $@
	! grep " 0|" $@ |grep -ve //UNREACHABLE
	echo PASS $@

.profraw.profdata:
	$(CPROF) merge -sparse $< -o $@

.unittest.profraw:
	LLVM_PROFILE_FILE=$@ ./$< >/dev/null

.c.unittest:
	$(CC) $(OPTS) $(DEFS) $(INCS) $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o *.unittest* *.profraw *.profdata *.coverage false_int
