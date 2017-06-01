CC = gcc
CC-AVR = avr-gcc
# release by default (no messages)
DEBUG=2
# optimization flags for AVR
#AVR_OPTIM = -O3
AVR_OPTIM = -Os -mcall-prologues
# whether or not we need wildcard expansion in Lulu (default = 0)
WILDCARD=0

ifneq ($(WILDCARD),0)
  WITH_EXPAND=build/wild_expand.o
  WITH_EXPAND_AVR=build_hex/wild_expand.o
endif

# path to the LULU P colony simulator
LULU_PCOL_SIM_PATH = /home/andrei/script_Python/lulu_pcol_sim
# path to the LULU P colony simulator script
LULU_PCOL_SIM = $(LULU_PCOL_SIM_PATH)/sim.py
# path to the lulu to C converter script
LULU_C = /home/andrei/script_Python/lulu_c/lulu_c.py
# path to one example instance file (can be set as an Environment variable to any Lulu formatted input file)
# 3 robots in the swarm
# 70 is the minimum kilo_uid
#LULU_INSTANCE_FILE = input_files/test_disperse.lulu pi_disperse 60 1
LULU_INSTANCE_FILE = input_files/pcol_decrement.lulu 60 1
# path to the Kilombo headers
KILOMBO_HEADERS = /usr/include
# path to the Kilolib headers -- only needed for AVR
KILOLIB_HEADERS = ../kilolib
# path to the Kilolib library -- only needed for AVR
KILOLIB_LIB = $(KILOLIB_HEADERS)/build/kilolib.a

ifeq ($(DEBUG),2)
	# release flags
	CFLAGS = -Wall -g -O2 -c -DPCOL_SIM -std=c99
	BFLAGS = -Wall -g -O2 -DPCOL_SIM
else
	#debug & testing flags
	CFLAGS = -Wall -g -O0 -fbuiltin -c -DPCOL_SIM -DDEBUG_PRINT=$(DEBUG) -std=c99
	BFLAGS = -Wall -g -O0 -fbuiltin -DPCOL_SIM -DDEBUG_PRINT=$(DEBUG)
endif

# AVR flags are not included in the above conditional because we simulateneously build both debug and release versions of the AVR library
CFLAGS_AVR = -c -mmcu=atmega328p -Wall -gdwarf-2 $(AVR_OPTIM) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DF_CPU=8000000 -I$(KILOLIB_HEADERS) -DKILOBOT
BFLAGS_AVR = -mmcu=atmega328p -Wall -gdwarf-2 $(AVR_OPTIM) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DF_CPU=8000000 -I$(KILOLIB_HEADERS) -DKILOBOT

CFLAGS_DEBUG_AVR = $(CFLAGS_AVR) -DDEBUG_PRINT=0 -Wl,-u,vfprintf -lprintf_min
BFLAGS_DEBUG_AVR = $(BFLAGS_AVR) -DDEBUG_PRINT=0 -Wl,-u,vfprintf -lprintf_min

all: build/simulator build/lulu.a hex

clean: clean_sim clean_autogenerated_lulu clean_hex

clean_sim:
	rm -vf build/*

clean_autogenerated_lulu:
	rm -vf src/instance.* src/rules.*

clean_hex:
	rm -vf build_hex/*

build/lulu.a: build/lulu.o build/rules.o build/wild_expand.o
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

build/wild_expand.o: src/wild_expand.h src/wild_expand.c
	$(CC) $(CFLAGS) src/wild_expand.c -o $@

# automatic generation of supported rules header and source (with string rule names)
src/rules.h src/rules.c:
	python $(LULU_PCOL_SIM) --ruleheader src/rules

# automatic generation of a C instance of a parsed P colony
src/instance.h src/instance.c:
	python $(LULU_C) $(LULU_INSTANCE_FILE) src/instance

# --------------------------------------------------------------------------------------------------------------------
# The following rules build for the kilobot

# we build 2 versions of the lulu library (with or without debug (i.e printf))
# the reason is that other applications can directly select the desired build, without the need of recompilation
hex: build_hex/lulu.a build_hex/lulu_debug.a

# WITH_EXPAND specifies whether we include wild_expand.o in lulu.a
build_hex/lulu.a: build_hex/lulu.o build_hex/rules.o $(WITH_EXPAND_AVR)
	ar rcs $@ $^

# WITH_EXPAND specifies whether we include wild_expand.o in lulu_debug.a
build_hex/lulu_debug.a: build_hex/lulu_debug.o build_hex/rules_debug.o $(WITH_EXPAND_AVR)
	ar rcs $@ $^

build_hex/lulu.o: src/lulu.h src/lulu.c src/rules.h
	$(CC-AVR) $(CFLAGS_AVR) src/lulu.c -o $@

build_hex/lulu_debug.o: src/lulu.h src/lulu.c src/rules.h
	$(CC-AVR) $(CFLAGS_DEBUG_AVR) src/lulu.c -o $@

build_hex/rules.o: src/rules.h src/rules.c
	$(CC-AVR) $(CFLAGS_AVR) src/rules.c -o $@

build_hex/rules_debug.o: src/rules.h src/rules.c
	$(CC-AVR) $(CFLAGS_DEBUG_AVR) src/rules.c -o $@

# only one wild_expand.o rule because wild_expand has no printf() included
build_hex/wild_expand.o: src/wild_expand.h src/wild_expand.c
	$(CC-AVR) $(CFLAGS_AVR) src/wild_expand.c -o $@
