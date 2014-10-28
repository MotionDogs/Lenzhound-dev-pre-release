BOARD = arduino:avr:leonardo

ifeq ($(OS),Windows_NT)
	PORT = com3
else
	PORT = /dev/ttyACM0
endif

SKETCHBOOK	:= $(realpath ../)
OPTS		:= 	--board $(BOARD) --port $(PORT) \
				--pref sketchbook.path=$(SKETCHBOOK) .

upload:
	arduino --upload $(OPTS)
verify:
	arduino --verify $(OPTS)