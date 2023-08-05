PROJECT:=rv32adventure
TARGETS:=emulator rv_app.bin

all: $(TARGETS)

RV_PREFIX:=riscv64-unknown-elf-

RV_CFLAGS:=-fno-stack-protector
RV_CFLAGS+=-static-libgcc -ffunction-sections
RV_CFLAGS+=-g -Os -march=rv32i -mabi=ilp32 -static
RV_LDFLAGS:= -T rv_app_src/flatfile.lds -nostdlib -Wl,--gc-sections -static-libgcc -lgcc

rv_app.elf : rv_app_src/main.c rv_app_src/start.S rv_app_src/barelibc.c
	$(RV_PREFIX)gcc -o $@ $^ $(RV_CFLAGS) $(RV_LDFLAGS)

rv_app.debug.txt : rv_app.elf
	$(RV_PREFIX)objdump -t $^ > $@
	$(RV_PREFIX)objdump -S $^ >> $@

rv_app.bin : rv_app.elf
	$(RV_PREFIX)objcopy $^ -O binary $@

emulator : vm_src/main.c vm_src/instructions.c vm_src/rv32i.c
	gcc -o $@ $^

test : emulator rv_app.bin
	emulator rv_app.bin