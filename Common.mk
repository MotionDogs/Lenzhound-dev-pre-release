BOARD = arduino:avr:leonardo
define greenecho
	@tput setaf 2
	@echo $1
	@tput sgr0
endef

ifeq ($(OS),Windows_NT)
	PORT = com3
else
	PORT = /dev/ttyACM0
endif

SKETCHBOOK	:= $(realpath ../)
OPTS		:= 	--board $(BOARD) --port $(PORT) \
				--pref sketchbook.path=$(SKETCHBOOK) .

verify:
	arduino --verify $(OPTS)
	$(call greenecho,"success!")
upload:
	arduino --upload $(OPTS)
	$(call greenecho,"success!")