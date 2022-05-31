#pragma once

#include "ofMain.h"

//	On Windows, if a COM port number exceeds 9, then it needs
//	to be prefaced with the "\\\\.\\" below. To take care of this,
//	the serial prefix will be added to all COM ports from the config file.
#define SERIAL_PREFIX ""
#ifdef _WIN32
#define SERIAL_PREFIX "\\\\.\\"
#endif

//	Defines the limit for the length of the buffer that can be received by the 
//	connected serial devices. If this length/index is exceeded while parsing serial
//	data, then the serial data will be ignored.
#define INTERACTIVE_DEVICE_BUFFER_LIMIT 64

//	Defines a custom location for the OpenFrameworks data folder.
#define DATA_FOLDER "../data"

//	The video folder provided by this definition should reside within the "data" folder
//	that is used by OpenFrameworks.#define VIDEO_FOLDER "video/"
#define VIDEO_FOLDER "video/"

class ofApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		static void wait(int i);
};

class InteractiveDevice 
{
public:
	int buffer_state;
	std::vector<unsigned char> buffer;
	ofSerial serial;
	ofVideoPlayer video;
	bool device_state;
	std::string port;
	int baud;
	bool is_restarting;

	void setup(const char* _port, int _baud, const char* _video_path) 
	{
		buffer_state = false;
		std::string temp_port = _port;
		port = SERIAL_PREFIX + temp_port;
		baud = _baud;

		if (serial.setup(port, _baud) == 0)
		{
			throw std::exception("\nFATAL ERROR! Error occured when trying to set up serial. Check config file and ensure serial ports are correct and available on the machine.");
		}

		if (video.load(_video_path) == false)
		{
			throw std::exception("\nFATAL ERROR! Error occured loading queue video, ensure video file is in data folder and that entry in config file is correct.");
		}

		device_state = false;
	}

	static void restartSerial(ofSerial _serial, std::string _port, int _baud)
	{
		if (_serial.isInitialized())
		{
			_serial.close();
		}

		_serial = ofSerial::ofSerial();

		/**
		* Yes it is odd that I am calling setup() here twice, but
		* for some reason it doesn't work when calling the function once,
		* it will just disconnect without trying to reconnect.
		*
		* restartSerial() in the InteractiveDevice class just calls close()
		* and setup() on the device object's serial member. Not much documentation
		* on ofSerial, so I don't know why it isn't closing and setting up with
		* a single call.
		*/
		ofApp::wait(1);
		std::cout << "Setup " << (_serial.setup(_port, _baud) ? "passed!" : "failed.") << std::endl;
	}

	void getStateFromSerial()
	{
		if (this->is_restarting)
		{
			return;
		}

		if (this->serial.available() <= 0)
		{
			return;
		}

		while (this->serial.available() > 0)
		{
			if (buffer.size() > INTERACTIVE_DEVICE_BUFFER_LIMIT)
			{
				std::cout << "Buffer received from " << port << " reached a length longer than 128 bytes and was ignored. Check that device is sending data in proper format." << std::endl;
				buffer.clear();
				return;
			}
			
			unsigned int byte = serial.readByte();

			if ((buffer_state == 0) && (byte == '['))
			{
				buffer_state = 1;
			}
			else if ((buffer_state == 1) && (byte != ']'))
			{
				buffer.push_back(byte);
			}
			else if ((buffer_state == 1) && (byte == ']'))
			{
				buffer_state = 0;
				device_state = buffer[1];
				buffer.clear();
			}
		}
	}
};
