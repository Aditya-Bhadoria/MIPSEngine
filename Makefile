CC = clang
CFLAGS = -Wall -Wextra -g
SRCS = src/main.c src/memory.c src/cpu.c src/elf_loader.c src/decoder.c src/predictor.c
OBJS = $(SRCS:.c=.o)
TARGET = MIPSEngine

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

# testing 

test-all: $(TARGET)
	@echo "\n[1/7] Generating binaries..."
	@python3 tests/generators/generate_elf.py
	@python3 tests/generators/generate_loop.py
	@python3 tests/generators/generate_hello.py
	@python3 tests/generators/generate_isa_test.py
	@python3 tests/generators/generate_math.py
	@python3 tests/generators/generate_rem.py
	
	@echo "\n[2/7] Running Dummy Test (Memory & CPU Init)"
	@./$(TARGET) tests/dummy.elf
	
	@echo "\n[3/7] Running Loop Test (Branch Predictor)"
	@./$(TARGET) --predictor=2bit tests/loop.elf
	
	@echo "\n[4/7] Running Hello World (Syscalls)"
	@./$(TARGET) tests/hello.elf
	
	@echo "\n[5/7] Running ISA Test (Jumps & Logic)"
	@./$(TARGET) tests/isa_test.elf
	
	@echo "\n[6/7] Running Math Test (HI/LO Registers)"
	@./$(TARGET) tests/math.elf
	
	@echo "\n[7/7] Running Comprehensive Edge Cases"
	@./$(TARGET) tests/rem.elf
	
	@echo "\n--- All Tests Passed! ---"