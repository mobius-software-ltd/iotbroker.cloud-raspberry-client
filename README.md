# iotbroker.cloud-raspberry-client
### Project description

IoTBroker.cloud Raspberry Pi Client is an application that allows you to connect to the server using MQTT, MQTT-SN, 
AMQP or COAP protocols. IoTBroker.cloud Raspberry Pi Client gives the opportunity to exchange messages using protocols 
mentioned above. Your data can be also encrypted with **TLS** or **DTLS** secure protocols.   

Below you can find a brief description of each protocol that can help you make your choice. 
If you need to get more information, you can find it in our [blog](https://www.iotbroker.cloud/clientApps/Raspberry%20Pi/MQTT).
 
**MQTT** is a lightweight publish-subscribe based messaging protocol built for use over TCP/IP.  
MQTT was designed to provide devices with limited resources an easy way to communicate effectively. 
You need to familiarize yourself with the following MQTT features such as frequent communication drops, low bandwidth, 
low storage and processing capabilities of devices. 

Frankly, **MQTT-SN** is very similar to MQTT, but it was created for avoiding the potential problems that may occur at WSNs. 

Creating large and complex systems is always associated with solving data exchange problems between their various nodes. 
Additional difficulties are brought by such factors as the requirements for fault tolerance, 
he geographical diversity of subsystems, the presence a lot of nodes interacting with each others. 
The **AMQP** protocol was developed to solve all these problems, which has three basic concepts: 
exchange, queue and routing key. 

If you need to find a simple solution, it is recommended to choose the **COAP** protocol. 
The CoAP is a specialized web transfer protocol for use with constrained nodes and constrained (e.g., low-power, lossy) 
networks. It was developed to be used in very simple electronics devices that allows them to communicate interactively 
over the Internet. It is particularly targeted for small low power sensors, switches, valves and similar components 
that need to be controlled or supervised remotely, through standard Internet networks.   
 
### Prerequisites 
* The Raspberry Pi model (It is recommended to take Raspberry Pi 3 Model B  ); 

* Micro SD card;

* Micro USB power supply;

* TV or monitor and HDMI cable (to use it as a desktop computer)

* Keyboard and mouse (to use it as a desktop computer)
### Installation 
* First, you should assemble the device following the instructions and need to make sure you have an operating system 
on your micro SD card. Raspbian, the Raspberry Pi Foundation’s official supported operating system;

* Plug your device into a monitor and attach a keyboard and mouse;  
* When you start your Raspberry Pi for the first time, the Welcome to Raspberry Pi application will pop up and guide you what to do.  

* Next you should install Jansson and Libwebsockets libraries following the instructions in links.  

* Then run the following command in the terminal to install libssl-dev secure Sockets Layer toolkit 
```
sudo apt-get install libssl-dev
```

* If the above-mentioned steps were successfully completed, you can clone the client using the following command in 
the terminal: 
```
git clone https://github.com/mobius-software-ltd/iotbroker.cloud-raspberry-client.git 
```
* Then run the following commands to build the client: 
```
cd iotbroker.cloud-raspberry-clien/Default/ 
```

```
make
```
* Note that iotbroker.cloud-raspberry-client and configuration file need to be in the same folder. 
To do it, please, copy configuration file to Default folder, using the following commands: 
```
cp config_file Default/ 
```
```
cd Default/
```
* Finally, you can run the application. 
```
./iotbroker.cloud-raspberry-client
```
* Open the configuration file and fill in fields using the following command. 
```
nano config_file  
```
Please note that at this stage it is not possible to register as a client. You can log in to your existing account.  

Now you are able to start exchanging messages with server.  

IoTBroker.Cloud C++ Client is developed by [Mobius Software](https://www.mobius-software.com/).

## [License](LICENSE.md)
