##############################################################################
# Title        : AVR Makefile for Windows
#
# Created      : Matthew Millman 2018-05-29
#                http://tech.mattmillman.com/
#
##############################################################################

# Fixes clash between windows and coreutils mkdir. Comment out the below line to compile on Linux
COREUTILS  = C:/Projects/coreutils/bin/

CLOCK      = 16000000
SRCS       = main.c cmd.c config.c util.c timeout.c timer.c usart_buffered.c stepper.c
OBJS       = $(SRCS:.c=.o)
DEPDIR     = deps
DEPFLAGS   = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
RM         = rm
MV         = mv
MKDIR      = $(COREUTILS)mkdir
LDFLAGS    = -Wl,-u,vfprintf -lprintf_flt -lm

ifeq ($(ARDUINO), LEONARDO)
DEVICE     = atmega32u4
CFLAGS     = -D_LEONARDO_
PROGRAMMER = -c arduino -P COM6 -c avr109 -b 57600 
else
DEVICE     = atmega328p
CFLAGS     = -D_UNO_
PROGRAMMER = -P COM5 -c arduino -b 115200  
endif

POSTCOMPILE = $(MV) $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os $(DEPFLAGS) $(CFLAGS) -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	steppercontroller.hex

.c.o:
	@$(MKDIR) -p $(DEPDIR)
	$(COMPILE) -c $< -o $@
	@$(POSTCOMPILE)

.S.o:
	@$(MKDIR) -p $(DEPDIR)
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
	@$(POSTCOMPILE)

.c.s:
	@$(MKDIR) -p $(DEPDIR)
	$(COMPILE) -S $< -o $@
	@$(POSTCOMPILE)

flash:	all
	$(AVRDUDE) -U flash:w:steppercontroller.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash

clean:
	$(RM) -f steppercontroller.hex steppercontroller.elf $(OBJS)

steppercontroller.elf: $(OBJS)
	$(COMPILE) -o steppercontroller.elf $(OBJS) $(LDFLAGS)

steppercontroller.hex: steppercontroller.elf
	avr-objcopy -j .text -j .data -O ihex steppercontroller.elf steppercontroller.hex

disasm:	steppercontroller.elf
	avr-objdump -d steppercontroller.elf

cpp:
	$(COMPILE) -E $(SRCS)

$(DEPDIR)/%.d:
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))