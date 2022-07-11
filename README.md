# M5Stack Binance Terminal
Access binance using your ESP32 based M5 Stack Core

## Getting Started

### Arduino IDE and M5Stack Core Libraries

Follow the instructions of the official [M5Stack website](https://docs.m5stack.com/en/quick_start/m5core/arduino) to install the latest Arduino IDE along with the required Libraries for your M5Stack.  
  
The high-level steps are outlined below:  

* Download and install the latest [Arduino IDE](https://www.arduino.cc/en/software)
* Open the Arduino IDE, select `File -> Preferences -> Settins`
* Copy the M5Stack board management URL below to your `Additional Development Board Manager URLs`  

```
https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json
```

* Next, in the Arduino IDE, select `Tools -> Board: "XXX" -> Development Board Manager...`
* In the Development Board Manager, search for `M5Stack` and install the latest package. (If the search yields no results, try restarting the Arduino IDE)

### ESP32 Filesystem Uploader

To upload the several files such as the hosted website or status icons shown on the screen you must install the ESP32 Filesystem Uploader.  

![ESP32 Filesystem Uploader context menu](/doc/img/esp32-fs-upload-menu.png)

* Navigate to the [Arduino ESP32 Filesystem plugin release page](https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/)
* Download the release .zip package.
* Find the Arduino IDE default Sketchbook folder, by default this is `C:\Users\<youruser>\Documents\Arduino`
  * You can check the location in Arduino IDE by selecting `File -> Preferences -> Settings`. The first prompt `Sketchbook location:` shows the location.
* Open this Sketchbook folder in your file explorer and create the subfolder `tools` if it does not exist yet.
* Extract the contents of the `ESP32FS-1.0.zip` into the `tools` folder.
* The folder structure should look like this: `<Sketchbookfolder>\tools\ESP32FS\tool\esp32fs.jar`
* Restart the Arduino IDE

### Dependencies

To compile the Arduino Sketch you must first install the dependencies (libraries) used in the project. Many of the libraries are available in the Arduino's IDE library manager. However, some must be downloaded and installed manually. A quick how-to and the download links are provided further below.

#### Libraries from the Arduino IDE library manager

To add these libraries, open Arduino IDE and select `Sketch -> Include Library -> Manage Libraries...` then search and install the corresponding library.

* [WebSockets - by Markus Sattler - V2.x.x](https://github.com/Links2004/arduinoWebSockets)
* [RingBuffer - by Jean-Luc Locoduino - V1.0.x](https://github.com/Locoduino/RingBuffer)
* [ArduinoJson - by Benoit Bilanchon - V6.x.x](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)

#### Libraries to install manually from .zip

To add these libraries, download the libraries as .zip using the links provided below (Click green `Code` button in GitHub and choose `Download ZIP`), then open Arduino IDE, select `Sketch -> Include Library -> Add .ZIP Library` and choose the downloaded .zip to add the library.  
  
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

### Board Configuration

Before compiling and uploading the Sketch, check the board settings below and adjust them accordingly (`Tools` Context menu in Arduino IDE):  
  
* Board: M5Stack-Core-ESP32  
* CPU Frequency: 240MHz (WiFi/BT)  
* Arduino Runs On: Core 1  
* Events Run On: Core 0  
* Port: (The COM Port your M5Stack is connected to)

### Upload Sketch

Before uploading the sketch, define the symbol of which you would like to see the binance charts of. The example below shows the BTCUSDT pair:  
```c
#define SYMBOL "btcusdt"
```  
Save the Sketch afterwards.  

Finally you can press the `Upload` Button in Arduino IDE. Wait for the Sketch to compile and upload.

### Sketch data upload

Once the Sketch upload was finished successfully, upload the sketch data (website and status icons). To do this, select `Tools -> ESP32 Sketch Data Upload`. Wait for the upload to finish.
  
### Connect M5Stack to local WiFi network

The first time you power up the M5Stack it will not be able to communicate with your local WiFi network. You must provide the credentials for the M5Stack to connect. Follow the steps below to do this:  

* Enable your computer's WiFi (or use your Smartphone).
* Connect your computer to the M5Stack-1 WiFi Network.
* Open a webbrowser and navigate to http://192.168.4.1/
> **Note**: If you are using your Smartphone, disable mobile data as the Smartphone will try to use the mobile data to connect because the M5Stack-1 WiFi Network is not connected to the internet.
* On the website provided, search the WLAN section and click `Settings`.
* Wait for the M5Stack to scan for WiFi Networks.
* Once you can see your WiFi Network, select it, enter your password and click `Connect`
* Wait for the M5Stack to connect
> **Note**: It may happen that the website shows `Connection Failed` even though the connection was established successfully. Just restart your M5Stack using its power button and check the screen for a local IP and the WiFi signal strenght icon.  

Once your M5Stack is connected to your local WiFi Network you may access the same website by navigating to `http://<m5stackip>` where `<m5stackip>` is the ip address shown on the M5Stack's screen at the top. You may now disconnect your computer or smartphone from the M5Stack-1 WiFi Network.  
  
**Concratulations, you are done with setting up your own M5Stack Binance Terminal!**

## Roadmap

### July 2022 - Exploration

Explore what's possible using the M5Stack and the Binance API

### August 2022 - Plots, Subplots & Indicators

* Add further Plots, such as 15m, 1h, 4h etc...  
* Implement first Subplot demonstration to show Volume  
* Include further indicators such as EMA  
* Improve Plot Scaling

### September 2022 - User Interface for Plots, Subplots & Indicators

Make all the available Plots and Indicators accessible for the user to change them on the fly using the M5Stack buttons or the Web-app hosted on the device.

