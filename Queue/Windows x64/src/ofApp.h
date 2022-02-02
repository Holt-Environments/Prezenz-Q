#pragma once

#include "ofMain.h"

#define SERIAL_PREFIX ""
#ifdef _WIN32
#define SERIAL_PREFIX "\\\\.\\"
#endif

#define INTERACTIVE_DEVICE_BUFFER_LIMIT 64
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

	void setup(const char* _port, int _baud, const char* _video_path) 
	{
		buffer_state = false;
		std::string temp_port = _port;
		port = SERIAL_PREFIX + temp_port;

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

	void getStateFromSerial()
	{
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