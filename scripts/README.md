### Scripts used in processing data for a robo car

* **capture_videos.sh** - This script runs on the RPi. Use it to capture videos with different positions and angles
* **video2images.sh** - Convert videos to image of required sizes, runs on server, processes data captured in 'capture_videos.sh' script
* **images2data.sh** - Create train, validation and test data directories that we can use to train the network, runs on server, uses image data generated in 'video2images.sh' script
* **serial_test.py** - Script to send commands over to arduino or alike using usb serial connection.

