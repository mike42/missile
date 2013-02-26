default:
	g++ src/missile.cpp examples/basic-sync/basic-sync.cpp -o bin/basic-sync -lusb-1.0 -Wall
	g++ src/missile.cpp examples/joystick-ctl/joystick-ctl.cpp examples/joystick-ctl/joystick/joystick.cpp -o bin/joystick-ctl -lusb-1.0 -std=c++11 -Wall

clean:
	rm bin/basic-sync
	rm bin/joystick-ctl
