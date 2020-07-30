##############################################################################
# Title        : AVR Makefile for Windows
#
# Created      : Matthew Millman 2018-05-29
#                http://tech.mattmillman.com/
#
##############################################################################

# Fixes clash between windows and coreutils mkdir. Comment out the below line to compile on Linux
COREUTILS  = C:/Projects/coreutils/bin/

DEVICE     = atmega32u4
CLOCK      = 16000000
PROGRAMMER = -c arduino -P COM3 -c avr109 -b 57600 
SRCS       = main.c cmd.c config.c util.c timeout.c usart_buffered.c stepper.c
OBJS       = $(SRCS:.c=.o)
FUSES      = -U lfuse:w:0x4F:m -U hfuse:w:0xC1:m -U efuse:w:0xff:m
DEPDIR     = deps
DEPFLAGS   = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
RM         = rm
MV         = mv
MKDIR      = $(COREUTILS)mkdir

POSTCOMPILE = $(MV) $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os $(DEPFLAGS) -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

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
	$(COMPILE) -o steppercontroller.elf $(OBJS)

steppercontroller.hex: steppercontroller.elf
	avr-objcopy -j .text -j .data -O ihex steppercontroller.elf steppercontroller.hex

disasm:	steppercontroller.elf
	avr-objdump -d steppercontroller.elf

cpp:
	$(COMPILE) -E $(SRCS)

$(DEPDIR)/%.d:
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))