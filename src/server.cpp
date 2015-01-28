//******************************************************************
//
// Copyright 2014 Intel Corporation.
// Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "config.h"

#include <csignal>
#include <functional>

#include "server.h"
#include "sensors.h"

using namespace Sensors;
using namespace std;
using namespace OC;


void IoTServer::initializePlatform()
{
  cout << "Running IoTServer constructor" << endl;

  m_platformConfig = make_shared<PlatformConfig>
    (ServiceType::InProc,
     ModeType::Server, "0.0.0.0",
     0,
     OC::QualityOfService::HighQos);

  OCPlatform::Configure(*m_platformConfig);
}

IoTServer::IoTServer(int pin,string key)
{
  initializePlatform();
  setupResources();
  m_ledRepresentation.setValue(key, 0);
  SetupPins(pin);
}

IoTServer::~IoTServer()
{
  cout << "Running IoTServer destructor" << endl;
  ClosePins();
}

void IoTServer::setupResources()
{
  EntityHandler cb3 = bind(&IoTServer::LEDEntityHandler, this, placeholders::_1);
  createResource(Config::m_endpoint, Config::m_type, cb3, m_ledResource);
}

void IoTServer::createResource(string Uri, string Type, EntityHandler Cb, OCResourceHandle& Handle)
{
  string resourceUri = Uri;
  string resourceType = Type;
  string resourceInterface = Config::m_interface;
  uint8_t resourceFlag = OC_DISCOVERABLE | OC_OBSERVABLE;

  OCStackResult result = OCPlatform::registerResource(Handle, 
                                                      resourceUri, resourceType,
                                                      resourceInterface, Cb, resourceFlag);

  if (result != OC_STACK_OK)
    cerr << "Could not create " << Type << " resource" << endl;
  else
    cout << "Successfully created " << Type << " resource" << endl;
}

void IoTServer::putLEDRepresentation()
{
  int state = 0;
  m_ledRepresentation.getValue(Config::m_key, state);
  SetOnboardLed(state);
  if (state == 0)
    cout << "Turned off LED" << endl;
  else if (state == 1)
    cout << "Turned on LED" << endl;
  else
    cerr << "Invalid request value" << endl;
}

OCRepresentation IoTServer::getLEDRepresentation()
{
  return m_ledRepresentation;
}


OCEntityHandlerResult IoTServer::LEDEntityHandler(shared_ptr<OCResourceRequest> Request)
{
  OCEntityHandlerResult result = OC_EH_ERROR;
  if (Request)
    {
      string requestType = Request->getRequestType();
      int requestFlag = Request->getRequestHandlerFlag();
      if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
          auto Response = std::make_shared<OC::OCResourceResponse>();
          Response->setRequestHandle(Request->getRequestHandle());
          Response->setResourceHandle(Request->getResourceHandle());
          if (requestType == "PUT")
            {
              cout << "PUT request for platform LED" << endl;
              OCRepresentation requestRep = Request->getResourceRepresentation();
              if (requestRep.hasAttribute(Config::m_key))
                {
                  try
                    {
                      requestRep.getValue<int>(Config::m_key);
                    }
                  catch (...)
                    {
                      Response->setResponseResult(OC_EH_ERROR);
                      OCPlatform::sendResponse(Response);
                      cerr << "Client sent invalid resource value type" << endl;
                      return result;
                    }
                }
              else
                {
                  Response->setResponseResult(OC_EH_ERROR);
                  OCPlatform::sendResponse(Response);
                  cerr << "Client sent invalid resource key" << endl;
                  return result;
                }
              m_ledRepresentation = requestRep;
              putLEDRepresentation();
              if (Response)
                {
                  Response->setErrorCode(200);
                  Response->setResourceRepresentation(getLEDRepresentation());
                  Response->setResponseResult(OC_EH_OK);
                  if (OCPlatform::sendResponse(Response) == OC_STACK_OK)
                    {
                      result = OC_EH_OK;
                    }
                }
            }
          else if (requestType == "GET")
            {
              cout << "GET request for platform LED" << endl;
              if (Response)
                {
                  Response->setErrorCode(200);
                  Response->setResourceRepresentation(getLEDRepresentation());
                  Response->setResponseResult(OC_EH_OK);
                  if (OCPlatform::sendResponse(Response) == OC_STACK_OK)
                    {
                      result = OC_EH_OK;
                    }
                }
            }
          else
            {
              Response->setResponseResult(OC_EH_ERROR);
              OCPlatform::sendResponse(Response);
              cerr << "Unsupported request type" << endl;
            }
        }
    }
  return result;
}

static int g_quit = 0;

void handle_signal(int signal)
{
  g_quit = 1;
}

int main(int argc, char* argv[])
{
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  sigaction(SIGINT, &sa, NULL);

  cout << "Server: "<<endl
       << "Press Ctrl-C to quit...." << endl;

  if (argc>1 && argv[1]) {
    Config::m_gpio  = atoi(argv[1]);
  }

  IoTServer server(Config::m_gpio,Config::m_key);

  do
    {
      usleep(2000000);
    }
  while (g_quit != 1);
  return 0;
}

