CC = gcc
CC-AVR = avr-gcc
DEBUG = 0

# path to the LULU P colony simulator
LULU_PCOL_SIM_PATH = /home/andrei/script_Python/lulu_pcol_sim
# path to the LULU P colony simulator script
LULU_PCOL_SIM = $(LULU_PCOL_SIM_PATH)/sim.py
# path to the lulu to C converter script
LULU_C = /home/andrei/script_Python/lulu_c/lulu_c.py
# path to one example instance file (can be set as an Environment variable to any Lulu formatted input file)
LULU_INSTANCE_FILE = $(LULU_PCOL_SIM_PATH)/input_files/input_ag_decrement.txt
# path to the Kilombo headers
KILOMBO_HEADERS = /usr/include
# path to the Kilolib headers -- only needed for AVR
KILOLIB_HEADERS = ../kilolib
# path to the Kilolib library -- only needed for AVR
KILOLIB_LIB = $(KILOLIB_HEADERS)/build/kilolib.a

ifeq ($(DEBUG),1)
  #debug & testing flags
  CFLAGS = -Wall -g -O0 -fbuiltin -c -DPCOL_SIM -DDEBUG_PRINT=0 -std=c99
  BFLAGS = -Wall -g -O0 -fbuiltin -DPCOL_SIM -DDEBUG_PRINT=0
else
  # release flags
  CFLAGS = -Wall -g -O2 -c -DPCOL_SIM -DDEBUG_PRINT=1 -std=c99
  BFLAGS = -Wall -g -O2 -DPCOL_SIM -DDEBUG_PRINT=1
endif

CFLAGS_AVR = -c -mmcu=atmega328p -Wall -gdwarf-2 -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DF_CPU=8000000 -I$(KILOLIB_HEADERS) -DKILOBOT
BFLAGS_AVR = -mmcu=atmega328p -Wall -gdwarf-2 -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DF_CPU=8000000 -I$(KILOLIB_HEADERS) -DKILOBOT

all: build/simulator build/lulu.a

clean:
	rm -vf build/*

clean_autogenerated_lulu:
	rm -vf src/instance.* src/rules.*

clean_hex:
	rm -vf build_hex/*

build/lulu.a: build/lulu.o build/rules.o
	ar rcs $@ $^

build/simulator: build/simulator.o build/instance.o build/lulu.a
	$(CC) $(BFLAGS) $^ -o $@

build/simulator.o: src/simulator.c src/instance.h src/rules.h
	$(CC) $(CFLAGS) src/simulator.c -o $@

build/instance.o: src/instance.h src/instance.c src/rules.h
	$(CC) $(CFLAGS) src/instance.c -o $@

build/lulu.o: src/lulu.h src/lulu.c src/rules.h
	$(CC) $(CFLAGS) src/lulu.c -o $@

build/rules.o: src/rules.h src/rules.c
	$(CC) $(CFLAGS) src/rules.c -o $@

# automatic generation of supported rules header and source (with string rule names)
src/rules.h src/rules.c:
	python $(LULU_PCOL_SIM) --ruleheader src/rules

# automatic generation of a C instance of a parsed P colony
src/instance.h src/instance.c:
	python $(LULU_C) $(LULU_INSTANCE_FILE) src/instance

# --------------------------------------------------------------------------------------------------------------------
# The following rules build for the kilobot

hex: build_hex/lulu.a

build_hex/lulu.a: build_hex/lulu.o build_hex/rules.o
	ar rcs $@ $^

build_hex/lulu.o: src/lulu.h src/lulu.c src/rules.h
	$(CC-AVR) $(CFLAGS_AVR) src/lulu.c -o $@

build_hex/rules.o: src/rules.h src/rules.c
	$(CC-AVR) $(CFLAGS_AVR) src/rules.c -o $@
