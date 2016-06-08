## About : ##

Minimal IoTivity example.


## Usage : ##

```

while true ; do date ; sleep 1 ;  done | ./server 

static int IoTServer::main(int, char**)
Server: 
Press Ctrl-C to quit....
IoTServer::IoTServer()
void IoTServer::init()
void IoTServer::setup()
void IoTServer::createResource(std::string, std::string, OC::EntityHandler, void*&)
Successfully created platform.data resource
void IoTServer::update(std::string&)
void IoTServer::putResourceRepresentation()
Tue 10 May 08:37:42 PDT 2016
void IoTServer::update(std::string&)
void IoTServer::putResourceRepresentation()
Tue 10 May 08:37:43 PDT 2016
(...)


./observer  2>/dev/null
(...)
Tue 10 May 08:37:50 PDT 2016
Tue 10 May 08:37:51 PDT 2016
(...)

```
