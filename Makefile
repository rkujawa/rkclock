PREFIX?=arm-none-eabi-
CC=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
BINDIR=./bin
BINARY=rkclock

all: realall.really

CFLAGS= -std=c11 -g3 -Os
CFLAGS+= -fno-common -ffunction-sections -fdata-sections
CFLAGS+= -I./libopencm3/include
CFLAGS+= -mcpu=cortex-m0plus -mthumb -msoft-float
# STM32L0 starts up with MSI at 2.1Mhz
CFLAGS+= -DSTM32L0 -DLITTLE_BIT=500000 

# -L./libopencm3/lib -lopencm3_stm32l0
LDFLAGS+=-Wl,--start-group -L./libopencm3/lib -lc -lgcc -lnosys -Wl,--end-group --static -nostartfiles -T src/nucleo-l011k4.ld 

##
BINARY_ELF+=$(BINDIR)/$(BINARY).elf
BINARY_BIN+=$(BINDIR)/$(BINARY).bin
BINARY_HEX+=$(BINDIR)/$(BINARY).hex

realall.really: outdir $(BINARY_ELF) $(BINARY_BIN) $(BINARY_HEX)

libopencm3/Makefile:
	@echo "Initializing libopencm3 submodule"
	git submodule update --init

libopencm3/lib/libopencm3_stm32l0.a: libopencm3/Makefile
	$(MAKE) V=1 TARGETS='stm32/l0' -C libopencm3

%.bin: %.elf
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

$(BINDIR)/%.o : src/%.c
	$(CC) -c -DRCC_LED1=RCC_GPIOB -DPORT_LED1=GPIOB -DPIN_LED1=GPIO3 \
	$(CFLAGS) $< -o $@

$(BINDIR)/$(BINARY).elf: $(BINDIR)/main.o libopencm3/lib/libopencm3_stm32l0.a
	$(CC) $(LDFLAGS) $(BINDIR)/main.o libopencm3/lib/libopencm3_stm32l0.a -o $@

outdir:
	mkdir -p $(BINDIR)

flash:
	openocd -f st_nucleo_l011k4.cfg -c "program $(BINARY_ELF) verify reset exit"

clean:
	$(RM) $(BINDIR)/*.o $(BINARY_ELF) $(BINARY_BIN) $(BINARY_HEX)

.PHONY: realall.really outdir clean all
#$(V).SILENT:
