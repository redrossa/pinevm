INC := include/*.h
SRC := src/*.c
DIR := ${CURDIR}
EXE := $(DIR)/pinevm

# Builds the VM executable and installs it
default:
	@make build
	@make install

# Compiles source files
build:
	@echo "Building executable..."
	@gcc $(SRC) -o $(EXE)

# Create an alias so the executable can be called directly on terminal as a command
install:
	@echo "Installing..."
	@sudo ln -s $(EXE) /usr/local/bin/pvm
	@chmod +x $(EXE)

# Deletes executable alias
uninstall:
	@echo "Uninstalling..."
	@rm /usr/local/bin/pvm

# Recompile binfile.c to create a bytecode test binary file
test:
	@gcc test/binfile.c -o binfile

# Uninstall and deletes VM executable in this directory
clean:
	@make uninstall
	@rm $(EXE)
