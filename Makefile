GPP=g++-4.8

default:
	mkdir -p bin
	$(GPP) src/missile.cpp examples/basic-sync/basic-sync.cpp -o bin/basic-sync -lpthread -lusb-1.0 -std=c++11 -Wall
	$(GPP) src/missile.cpp examples/joystick-ctl/joystick-ctl.cpp examples/joystick-ctl/joystick/joystick.cpp -lpthread -o bin/joystick-ctl -lusb-1.0 -std=c++11 -Wall
	$(GPP) src/missile.cpp examples/keyboard-ctl/keyboard-ctl.cpp -o bin/keyboard-ctl -lpthread -lusb-1.0 -lncurses -std=c++11 -Wall

clean:
	rm -f bin/basic-sync
	rm -f bin/joystick-ctl
	rm -f bin/keyboard-ctl
