Holt Environments Prezenz-Q
Author: Anthony Mesa
============================

To add a new controller to the experience, first visit
bluetooth settings on the device and connect to the 
desired controller. Then in Windows 10 bluetooth settings
choose 'more bluetooth options'. In the dialogue window that
shows up, switch to the COM ports tab and you should see the
two ports that are listed for the newly connected controller.
Take note of the 'Outgoing' COM port, and in the config.json
file in the data folder, add a new json entry to "sensors".

e.g.

	"sensors": {
		"arbitrary_name": {
			"port": "COM13";
			"video": "theAssociatedVideo.mp4"
		}
	}

All videos that are referenced in the sensors section, and referenced
via 'background' must be residing in the video folder in the data
directory.

