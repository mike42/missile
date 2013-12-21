#include "../../src/missile.h"
#include "joystick/joystick.h"
#include <thread>

#define JS_THOLD 16384

using namespace std;

/* A couple of globals to pass things around */
Missile *launcher;
bool finished;

/**
 * Fired on every joystick event
 */
void gotEvent(struct js_event e, bool *finalEvent) {
	*finalEvent = false;
	/* Handle button inputs only. Directional movement is polled. */
	if(e.type == JS_EVENT_BUTTON && e.value == 1) {
		switch(e.number) {
			case 0:
				launcher -> fire();
				break;
			case 1:
				*finalEvent = true;
				finished = true;
				launcher -> stop();
				break;
		}
	}
}

/**
 * Start a loop for joystick input
 */
void followJoystick(Joystick* input, Missile *newLauncher) {
	launcher = newLauncher;
	input -> getInput();
	cout << "input stopped\n";
}

/**
 * USB code and main loop
 */
int main() {
	uint16_t vendor_id = 0x0a81;
	uint16_t product_id = 0x0701;
	libusb_device_handle* launcherHandle;

	/* Verify presence of joystick */
	Joystick *input = new Joystick("/dev/input/js0", gotEvent);
	if(!input -> connected()) {
		return 1;
	}

	/* Open rocket launcher */
	libusb_init(NULL);
	libusb_set_debug ( 0, 3 );
	launcherHandle = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
	if(launcherHandle == NULL) {
		cout << "No missile launcher found\n";
		return 1;
	}

	/* Initialise Missile object and get input in another thread */
	Missile *launcher = new Missile(launcherHandle);
	thread t1(followJoystick, input, launcher);

	while(!finished) {
		launcher -> check_state();

		if(input -> axis(0) > JS_THOLD) {
			launcher -> move(ML_RIGHT);
		} else if(input -> axis(0) < -JS_THOLD) {
			launcher -> move(ML_LEFT);
		} else if(input -> axis(1) > JS_THOLD) {
			launcher -> move(ML_DOWN);
		} else if(input -> axis(1) < -JS_THOLD) {
			launcher -> move(ML_UP);
		}

		usleep(100000);
	}

	t1.join();

	delete launcher;

	libusb_exit(NULL);

	return 0;
}

