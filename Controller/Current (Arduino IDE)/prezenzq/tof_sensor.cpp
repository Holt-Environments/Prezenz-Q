
/**
   =======================================================================
   PrezenzQ Controller - tof_sensor.cpp

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   =======================================================================
*/

#include "tof_sensor.h"

//  Object for working with TOF sensor
static VL53L1X sensor;
static int sensor_initialized;

/**
 * Initializes the sensor and I2C library that allows us to interface
 * with the sensor.
 *
 * Technically this function could return boolean values, but using ints means
 * we can catch or pass other error codes in the future if we need to.
 *
 * Return value is -1 if the sensor fails to initialize,
 * and is 0 otherwise.
 */
int tof_sensor_init()
{
	Wire.begin();
	Wire.setClock(SENSOR_I2C_CLOCK);

	sensor.setTimeout(SENSOR_TIMEOUT);

	sensor_initialized = sensor.init();

	sensor.setROISize(4, 4);

	if (sensor_initialized)
	{
		/* For more information on sensor related functions check out the API documentation
		 * here: https://www.pololu.com/file/0J1507/VL53L1X-UM2356.pdf
		 */
		sensor.setDistanceMode(VL53L1X::Short);
		sensor.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET);
		sensor.startContinuous(SENSOR_WAIT);
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief Simply makes a check to determine if the sensor value lies within
 * the range defined by SENSOR_TRIGGER_DISTANCE.
 *
 * @param _sensor_value Current distance reading from TOF sensor.
 * @return true An object is detected within the range of SENSOR_TRIGGER_DISTANCE.
 * @return false No objects detected within the range of SENSOR_TRIGGER_DISTANCE.
 */
bool object_detected_in_range(int _sensor_value)
{
	return _sensor_value < SENSOR_TRIGGER_DISTANCE;
}

/**
 * @brief Check to see if the detection state of the sensor has
 * changed. If so, run the appropriate event handlers on_detected,
 * and on_not_detected that should be set during init().
 */
int tof_sensor_update()
{
	static char receipt;
	static bool detection_state;
	static int detection_buffer;
	static bool trigger_state;
	static int sensor_value;
	static int previous_signal_status_id;

	/* The TOF should be running in continuous mode after being setup
	 * in init(). Since it is reading continuously, we need to query
	 * that new data is available from the sensor before we try to read
	 * any new info. If new data is not available, we don't return, rather
	 * we continue normally, evaluating with the value that was placed
	 * into the static sensor_value last time.
	 */
	if (sensor.dataReady())
	{
		sensor_value = sensor.read(false);
	}

	/* Catch the event that the sensor times out. If that happens, return.
	 */
	if (sensor.timeoutOccurred())
	{
		return -1;
	}

	int signal_status_id = sensor.ranging_data.range_status;
	String signal_status = VL53L1X::rangeStatusToString(sensor.ranging_data.range_status);

	bool is_error;

	if (signal_status_id != previous_signal_status_id)
	{
		Serial.print(signal_status);
		Serial.print("- sensor value: ");
		Serial.println(sensor_value);
		previous_signal_status_id = signal_status_id;
	}

	/* Catch the event that the status ID is not 0. Given an error occurrs ("sensor fail", etc.)
	 * when users walk away from the sensor perpendicular to its line of sight, in that case, this
	 * resets the detection state to false.
	 */
	if (signal_status.equals("signal fail"))
	{
		detection_buffer = 0;

		if ((trigger_state == true))
		{
			return 1;
		}
		/* The only signal status ID we are interested in is 0, which
		 * signifies that the sensor made a valid reading. All other status's
		 * with their associated sensor readings should be ignored. If the TOF
		 * currently has a true trigger state, and the status id becomes non-zero,
		 * then that trigger state needs to return to false.
		 */
	}
	else if (signal_status_id == 0)
	{

		/* Update the detection state given reading is in range or not.
		 */
		detection_state = object_detected_in_range(sensor_value);

		/* If the detections state is true, Increment the detection buffer.
		 * If the detection state is not true then reset the detection buffer
		 * to 0. This detection buffer implementation provides a slight wait
		 * time before the sensor recognizes something in range. This keeps
		 * things like people walking by the sensor from falsely triggering it.
		 */
		if (detection_state)
			detection_buffer++;
		else if (!detection_state)
			detection_buffer = 0;

		/* If trigger state is false, but the detection buffer exceeds the max,
		 * then turn the trigger state on and run the on_detected handler function.
		 */
		if ((trigger_state == false) && (detection_buffer > SENSOR_DETECTION_BUFFER_MAX))
		{
			trigger_state = true;
			return 0;
		}

		/* If the trigger state is currently true, but the detection buffer has been
		 * reset to 0, then flip the trigger state to false and run the on_undetected handler function.
		 */
		else if ((trigger_state == true) && (detection_buffer == 0))
		{
			trigger_state = false;
			return 1;
		}
	}
}
