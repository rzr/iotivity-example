// -*-c++-*-
//******************************************************************
//
// Copyright 2014 Intel Corporation.
// Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
// Copyright 2016 Samsung <philippe.coval@osg.samsung.com>
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

std::string Config::m_interface = "iotivity.example.resources";
std::string Config::m_type = "platform.led";
std::string Config::m_endpoint = "/led";
std::string Config::m_key = "switch";

/* default will work on minnowmax calamari's LED3 **/
unsigned int Config::m_gpio = 21;

#if 1
#include <dlog.h>
#else
#define dlog_print(type,tag,message) printf(message)
#endif

// 	Config::log(__PRETTY_FUNCTION__);

void Config::log(char const * const message)
{
	char const * const LOGTAG="LOG";
	dlog_print(DLOG_INFO, LOGTAG, message);
	printlog(message);
}
