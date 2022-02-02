
#include "ofApp.h"
#include <cmath>
#include <cassert>
#include <ofJson.h>

ofVideoPlayer background;
ofVideoPlayer* overlay;

std::vector<InteractiveDevice*> device_list;
std::vector<ofVideoPlayer*> video_queue;

int overlay_fade_out_begin;
int overlay_fade_out_end;
int fade_duration;
int window_width;
int window_height;
int window_posx;
int window_posy;
int framerate;

bool fatal_error = false;

void queueAdd(InteractiveDevice* _device)
{
	video_queue.push_back(&(_device->video));
	InteractiveDevice* _corresponding_device = NULL;

	if (video_queue.size() > 1)
	{
		_device->serial.writeBytes("W", 1);
	}
	else
	{
		_device->serial.writeBytes("N", 1);
	}
}

void queueRemove(InteractiveDevice* _device)
{
	ofVideoPlayer* prev_queue_head = video_queue[0];

	for (int i = 0; i < video_queue.size(); i++)
	{
		if (video_queue[i] == &(_device->video))
		{
			_device->serial.writeBytes("F", 1);
			video_queue.erase(video_queue.begin() + i);
		}
	}

	if (prev_queue_head != video_queue[0])
	{
		for (auto i : device_list)
		{
			if (&(i->video) == video_queue[0])
			{
				i->serial.writeBytes("N", 1);
			}
		}
	}
}

void handleVideoState(InteractiveDevice * _device)
{
	bool video_in_list = false;
	for (auto i : video_queue)
	{
		if (i == &(_device->video))
		{
			video_in_list = true;
		}
	}

	if (!video_in_list && _device->device_state == true)
	{
		queueAdd(_device);
	}
	else if (video_in_list && _device->device_state == false)
	{
		queueRemove(_device);
	}
}

bool isNumber(std::string _string)
{
	for (auto i : _string)
	{
		if ((i != '.') && (std::isdigit(i) == false))
		{
			return false;
		}
	}
	return true;
}

void loadConfigFile()
{
	ofJson file;
	try {
		file = ofLoadJson("config.json");
	}
	catch (std::exception)
	{
		std::cout << "\nFATAL ERROR! Config file not found in data folder, startup aborted." << std::endl;
		fatal_error = true;
		exit(-1);
	}

	try {
		int count = 0;

		std::string framerate_s = file["framerate"];
		if (!isNumber(framerate_s)) { throw std::exception("In config.json, \"framerate\" must be an integer."); }
		framerate = (int)atoi(framerate_s.c_str());

		std::string width_s = file["width"];
		if (!isNumber(width_s)) { throw std::exception("In config.json, \"Width\" must be an integer."); }
		window_width = (int)atoi(width_s.c_str());

		std::string height_s = file["height"];
		if (!isNumber(height_s)) { throw std::exception("In config.json, \"height\" must be an integer."); }
		window_height = (int)atoi(height_s.c_str());

		std::string posx_s = file["posx"];
		if (!isNumber(posx_s)) { throw std::exception("In config.json, \"posx\" must be an integer."); }
		window_posx = (int)atoi(posx_s.c_str());

		std::string posy_s = file["posy"];
		if (!isNumber(posy_s)) { throw std::exception("In config.json, \"posy\" must be an integer."); }
		window_posy = (int)atoi(posy_s.c_str());

		std::string fade_duration_s = file["fade_duration"];
		if (!isNumber(fade_duration_s)) { throw std::exception("In config.json, \"fade_duration\" must be a float."); }
		fade_duration = (float)atof(fade_duration_s.c_str()) * 60;

		std::string background_video = file["background"];
		try {
			background.load(VIDEO_FOLDER + background_video);
		}
		catch (std::exception e)
		{
			std::cout << "\nFATAL ERROR! Error occured loading background video, ensure video file is in './data/" << VIDEO_FOLDER << "' folder and that entry in config file is correct." << std::endl;
			throw -1;
		}

		for (ofJson i : file["sensors"])
		{
			InteractiveDevice* temp_device = new InteractiveDevice();

			std::string temp_port = i["port"];
			std::string temp_port_file =  temp_port;

			std::string temp_video_file = VIDEO_FOLDER;
			std::string temp_video = i["video"];
			temp_video_file.append(temp_video);
		
			try {
				temp_device->setup(temp_port_file.c_str(), 9600, temp_video_file.c_str());
			}
			catch (std::exception e)
			{
				std::cout << e.what() << std::endl;
				throw -1;
			}
			 
			device_list.push_back(temp_device);
		}
	}
	catch (int e)
	{
		fatal_error = true;
		exit(-1);
	}
	catch (std::exception e) // this catches all other errors, so if syntax of json is correct, look for another issue in the code.
	{
		std::cout << e.what() << std::endl;
		std::cout << "\nFATAL ERROR! Config file contains a syntax error. Please refer to documentation for syntax info." << std::endl;
		fatal_error = true;
		exit(-1);
	}
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetDataPathRoot("../data");

	loadConfigFile();

	ofSetWindowShape(window_width, window_height);
	ofSetWindowPosition(window_posx, window_posy);
	ofSetFrameRate(framerate);

	background.play();
}

bool isCurrentFrameOutsideFadePeriod()
{
	int current_frame = overlay->getCurrentFrame();
	bool frame_in_begin_fade = (current_frame >= 0) && (current_frame <= fade_duration);
	bool frame_in_end_fade = (current_frame >= overlay_fade_out_begin) && (current_frame <= overlay_fade_out_end);
	return (!frame_in_begin_fade && !frame_in_end_fade);
}

bool isCurrentFrameLastFrameOfFade()
{
	int current_frame = overlay->getCurrentFrame();
	return (current_frame == overlay_fade_out_end);
}

void updateFadeOut()
{
	overlay_fade_out_begin = overlay->getCurrentFrame() + 1;
	overlay_fade_out_end = overlay_fade_out_begin + fade_duration;
}

void updateDevices()
{
	for (auto device : device_list)
	{
		device->getStateFromSerial();
		handleVideoState(device);
	}
}

void updateVideoQueue()
{
	if (video_queue.empty())
	{
		if (overlay != NULL) {
			bool current_frame_outside_fade_period = isCurrentFrameOutsideFadePeriod();
			bool current_frame_is_last_frame_of_fade = isCurrentFrameLastFrameOfFade();

			if (current_frame_outside_fade_period)
			{
				updateFadeOut();
			}
			else if (current_frame_is_last_frame_of_fade)
			{
				overlay = NULL;
			}
		}
	}
	else
	{
		if (overlay != NULL)
		{
			bool current_frame_outside_fade_period = isCurrentFrameOutsideFadePeriod();
			bool current_frame_is_last_frame_of_fade = isCurrentFrameLastFrameOfFade();

			if (current_frame_outside_fade_period)
			{
				bool head_changed_since_playing_overlay = (overlay != video_queue[0]);

				if (head_changed_since_playing_overlay)
				{
					updateFadeOut();
				}
			}
			else if (current_frame_is_last_frame_of_fade)
			{
				for (auto i : device_list)
				{
					if (overlay == &i->video)
					{
						i->device_state = false;
						handleVideoState(i);
					}
				}

				overlay = NULL;
			}
		}
		else
		{
			overlay = video_queue[0];
			overlay->setLoopState(OF_LOOP_NONE);
			overlay->firstFrame();
			overlay_fade_out_end = overlay->getTotalNumFrames();
			overlay_fade_out_begin = overlay_fade_out_end - fade_duration;
			overlay->play();
		}
	}
}

void updateBackground()
{
	if (overlay != NULL)
	{
		overlay->update();

		bool current_frame_outside_fade_period = isCurrentFrameOutsideFadePeriod();
		bool current_frame_is_last_frame_of_fade = isCurrentFrameLastFrameOfFade();

		if (current_frame_outside_fade_period)
		{
			background.setPaused(true);
		}
		else
		{
			background.play();
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	background.update();

	updateDevices();
	updateVideoQueue();
	updateBackground();
}

void setOverlayFrameOpacity()
{
	int frame_number = overlay->getCurrentFrame();

	if ((frame_number > -1) && (frame_number <= fade_duration))
	{
		int opacity = (int)(((double) frame_number / fade_duration) * 255);
		ofSetColor(255, 255, 255, opacity);
	}
	else if ((frame_number <= overlay_fade_out_end) && (frame_number >= overlay_fade_out_begin))
	{
		int numerator = frame_number - overlay_fade_out_begin;
		int denominator = overlay_fade_out_end - overlay_fade_out_begin;
		double lerp_value = (double) numerator / denominator;

		int opacity = (1.0 - lerp_value) * 255;
		ofSetColor(255, 255, 255, opacity);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	background.draw(window_posx, window_posy, window_width, window_height);

	if (overlay != NULL)
	{
		ofEnableAlphaBlending();
		setOverlayFrameOpacity();
		overlay->draw(window_posx, window_posy, window_width, window_height);
		ofDisableAlphaBlending();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 27) {
		exit();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::exit()
{
	for (InteractiveDevice* i : device_list)
	{
		free(i);
	}

	if (fatal_error == true)
	{
		std::cout << "\nPress enter to exit..." << std::endl;
		std::cin.ignore();
	}
}
