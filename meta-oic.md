
IoTivity is Getting Ready for Automotive

Samsung has recently intensified its focus on improving the driving experience with the launch of Connect Auto. As a coincidence, I tried to put together different efforts from our group to into an open connected car experiment.

Check out this video which shows a DIY Tizen fan thats controlled from an Automotive Linux system and Tizen TM1 mobile device:
Its All About Interoperability

It might look a bit over-engineered with all of the devices in this demonstration, but the purpose is to validate communication between devices on a network (WiFi, BT, BLE) in various contexts. Of course, its possible to adapt the DIY fan to use an automatic gate mechanism or something else to interact with the driver, or it could even use cloud services that ensure the necessary level of security.

IoTivity is Getting Ready for Automotive - WHise0EeSnD20

Interoperability is the key word here, I believe were headed in the right direction following the recent Open Connectivity Foundation announcement:

Open Connectivity Foundation Brings Massive Scale to IoT Ecosystem : The OCF unifies the entirety of the former Open Interconnect Consortium with leading companies at all levels  silicon, software, platform, and finished-goods  dedicated to providing this key interoperability element of an IoT solution.
IoTivity and Automotive Grade Linux

If youre interested in trying this out, its easy to rebuild the AGL distro with IoTivity included using a single make command, after cloning my meta-yocto-demos helper project. Check instructions details on this dedicated page. If you happen to be attending the Automotive Grade Linux Member Meeting, there will be an Open Interconnect Consortium & AGL presentation where you can learn more.

IoTivity is Getting Ready for Automotive - QGwKcZ9SuNxxC
Yocto as the Base of GENIVI and Tizen

Id like also to add that the Yocto project has become quite popular among open source automotive projects. Personally, I like it because its design encourages cooperation between operating systems, and it has the potential to become a point of convergence. For example, I managed to ship meta-oic into the GENIVI demo platform without any adaptation, I suppose its likely the same for any OpenEmbedded-based distro.

The final item of the demo is the IoTivity server running on a Tizen-powered RaspberryPI 1, for more details on this system, check out the presentation I gave about Tizen Yocto at FOSDEM earlier this year with current maintainer Leon Anavi:

Connected Tizen: Bringing Tizen to Your Connected Devices Using the Yocto Project from Samsung Open Source Group
More to Come

If youre curious about OSVehicle, and willing to contribute to an open source, connected car, join me in the OSVehicle forum. As you can observe from this example, Yocto is a common denominator of these systems and it makes IoTivity ready to reach many devices including automotive, smart homes, and more. Im hoping to begin work on adding IoTivity support to GENIVI and AGL soon, which could help bring great features like car-to-car connectivity.

IoTivity is Getting Ready for Automotive - iotivity-minnowmax.gif

Last but not least, Id like to invite you to my talk  that explains How to use IoTivity on Tizen at the OpenIoT Summit in in April; dont miss the other sessions from Samsung speakers.
