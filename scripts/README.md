### Scripts used in processing data for a robo car

* **capture_videos.sh** - This script runs on the RPi. Use it to capture videos with different positions and angles
* **video2images.sh** - convert videos to image of required sizes
* **images2data.sh** - Create train, validation and test data directories that we can use to train the network
* **serial_test.py** - Script to send commands to arduino or alike using usb serial connection
