#include "missile.h"

/**
 * Based on an open libusb handle, detach the kernel driver and claim device's interface
 */
Missile :: Missile(libusb_device_handle* launcherHandle) {
	observeLimits = true;
	async = true;
	state_expires = false;	

	handle = launcherHandle;
	libusb_detach_kernel_driver(handle, 0);
	libusb_claim_interface(handle, 0);

	/* Initialise to a stopped state */
	state = ML_STOP;
	ml_set_state(state);
}

/**
 * Destructor just needs to close the USB handle
 */
Missile :: ~Missile() {
	set_state(ML_STOP);

	libusb_close(handle);
}

/**
 * Set state with expiry in milliseconds
 */
void Missile :: set_state(unsigned char cmd, long expiry) {
	/* Time calculation if necessary */
	state_expires = (expiry > 0);
	if(state_expires) {
		/* Figure out a timespec object for when this will expire */
		long ns = (expiry * 1000000) % 1000000000;
		long sec = expiry / 1000;
		timespec state_start;
		clock_gettime(CLOCK_MONOTONIC, &state_start);
		state_expiry.tv_sec = state_start.tv_sec + (sec);
		state_expiry.tv_nsec = state_start.tv_nsec + (ns);
	}

	/* Update state if necessary */
	state = cmd;
	if(observeLimits) {
		if(state == cmd && cmd != ML_FIRE && cmd != ML_STOP) {
			/* Flick to a stop state then back to movement.
				This allows limits to be observed to prevent hardware damage */
			ml_set_state(ML_STOP);
		}
		ml_set_state(cmd);
	} else {
		if(state != cmd) {
			/* Only change state as required */
			ml_set_state(cmd);
		}
	}
}

/**
 * Set state without expiry
 */
void Missile :: set_state(unsigned char cmd) {
	set_state(cmd, -1);
}

/**
 * If this returns true, then it should be called in a loop if you want an
	expiring state to stop.
 **/
bool Missile :: check_state() {
	/* Check if state has expired */
	if(state_expires) {
		timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		if((ts.tv_sec == state_expiry.tv_sec && ts.tv_nsec >= state_expiry.tv_nsec)
			|| ts.tv_sec > state_expiry.tv_sec) {
			/* Event has expired */
			stop();
		}
	}

	return state_expires;
}

/**
 *  Send new command to missile launcher.
 */
void Missile :: ml_set_state(unsigned char cmd) {
	unsigned char buffer[1];
	buffer[0] = cmd;
	libusb_control_transfer(handle, LIBUSB_REQUEST_TYPE_STANDARD, LIBUSB_REQUEST_SET_CONFIGURATION, 0, 0, buffer, 1, 1000); 
}

/**
 *  Move the launcher (or set it in any state, actually) for a given duration, or -1 to leave it in the state.
 */
void Missile :: move(unsigned char direction, long duration) {
	set_state(direction, duration);
	if(!async) {
		wait();
	}
}

/**
 *  Move the launcher (or set it in any state, actually) for the default move duration
 */
void Missile :: move(unsigned char direction) {
	move(direction, ML_MOVE_TIME);
}

void Missile :: fire() {
	set_state(ML_FIRE, ML_FIRE_TIME);
	if(!async) {
		wait();
	}
}

/**
 * Stop any movement or firing which is taking place
 */
void Missile :: stop() {
	if(state == ML_FIRE) {
		/* The launcher wont stop from firing, so flick through a short move */
		ml_set_state(ML_UP);
	}
	ml_set_state(ML_STOP);
	state = ML_STOP;
	state_expires = false;
}

/**
 *  Wait for any times actions to complete. This is called after fire() or move() if async = false.
 */
void Missile :: wait() {
	while(check_state()) {
		usleep(ML_MOVE_TIME * 1000);
	}
}
