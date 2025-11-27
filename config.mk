TARGET:=riscv64-unknown-elf-
CC:=$(TARGET)gcc
LD:=$(TARGET)ld
CFLAGS:=-ffreestanding -std=c11 -Wall -Wextra -O0 -ggdb -gdwarf-2 -mcmodel=medany -Ilibc/include
LDFLAGS:=-z max-page-size=4096
