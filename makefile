BOARD = arduino:avr:uno
PORT = /dev/ttyACM0

upload:
	arduino-cli compile --fqbn $(BOARD) .
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) .
