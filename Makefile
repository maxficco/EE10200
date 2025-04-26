BOARD=arduino:avr:uno
PORT=/dev/cu.usbmodem1101
SKETCH=bopit

all: compile upload

compile:
	arduino-cli compile --fqbn $(BOARD) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) $(SKETCH)

clean:
	rm -rf build

