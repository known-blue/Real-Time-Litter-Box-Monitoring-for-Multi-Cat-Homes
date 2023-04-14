# Real-Time-Litter-Box-Monitoring-for-Multi-Cat-Homes
Utilizes an Adafruit ESP32, a Raspberry Pi with HQ Camera and a Windows desktop with a GPU


## To run: 

#### Raspberry Pi
 - Put the RaspPi folder on your Raspberry Pi
 - Compile and run `getESPData.cpp`
      - `g++ getESPData.cpp -o ESP`
      - `./ESP`
 
#### Windows
 - Put the Windows folder on your machine
      - **Make sure you have OpenCV and Tensorflow installed!**
           - `pip install cv2`
           - `pip install tensorflow`
 - Run `startDetection.py`
      - `python3 startDetection.py`
 
 #### ESP32 
 - This is built using PlatformIO 
      - Import the ESP32 folder as a new project in PlatformIO, build the project then upload to ESP32
      - ***NOTE:*** Wait to upload/run this until the RaspberryPI and Windows machines are running!! (might cause issues)
