# //******************************************************************
# // Copyright 2016 Samsung Electronics France SAS All Rights Reserved.
# //******************************************************************
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

# Configuration settings

# ARTIK10/Pin13
#CPPFLAGS+=-DCONFIG_GPIO=22
#CPPFLAGS+=-DCONFIG_LOG=1

USER?=$(shell echo '${USER}' || echo default)

-include config-user-${USER}.mk
