PREFIX?=arm-none-eabi-
CC=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
BINDIR=bin
SRCDIR=src
BINARY=rkclock

OPENCM3_DIR=./libopencm3
OPENCM3_LIBDIR=$(OPENCM3_DIR)/lib
OPENCM3_BIN=$(OPENCM3_LIBDIR)/libopencm3_stm32l0.a

CFLAGS= -std=c17 -Os -Wall
CFLAGS+= -g3
CFLAGS+= -fno-common -ffunction-sections -fdata-sections
CFLAGS+= -I$(OPENCM3_DIR)/include
CFLAGS+= -mcpu=cortex-m0plus -mthumb -msoft-float
# STM32L0 starts up with MSI at 2.1Mhz
CFLAGS+= -DSTM32L0

#  -lopencm3_stm32l0
LDSCRIPT=$(SRCDIR)/nucleo-l011k4.ld
LDFLAGS+=-Wl,--start-group -L$(OPENCM3_LIBDIR) -lc -lgcc -lnosys -mcpu=cortex-m0plus -Wl,--end-group --static -nostartfiles -T $(LDSCRIPT)

##
BINARY_ELF+=$(BINDIR)/$(BINARY).elf
BINARY_BIN+=$(BINDIR)/$(BINARY).bin
BINARY_HEX+=$(BINDIR)/$(BINARY).hex

OBJS=$(BINDIR)/main.o $(BINDIR)/tm1637.o $(BINDIR)/usart.o

all: outdir $(BINARY_ELF) $(BINARY_BIN) $(BINARY_HEX)

libopencm3/Makefile:
	@echo "Initializing libopencm3 submodule"
	git submodule update --init

$(OPENCM3_BIN): $(OPENCM3_DIR)/Makefile
	$(MAKE) V=1 TARGETS='stm32/l0' -C $(OPENCM3_DIR)

%.bin: %.elf
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

$(BINDIR)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BINDIR)/$(BINARY).elf: $(OBJS) $(OPENCM3_BIN)
	$(CC) $(LDFLAGS) $(OBJS) $(OPENCM3_BIN) -o $@

outdir:
	mkdir -p $(BINDIR)

flash:
	openocd -f st_nucleo_l011k4.cfg -c "program $(BINARY_ELF) verify reset exit"

serial-console:
	cu -s 9600 -l /dev/ttyACM0

clean:
	$(RM) $(BINDIR)/*.o $(BINARY_ELF) $(BINARY_BIN) $(BINARY_HEX)

.PHONY: outdir clean flash serial-console all
#$(V).SILENT:
