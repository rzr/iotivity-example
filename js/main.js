#!/usr/bin/env node
// Copyright 2017 Samsung Electronics France SAS
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

function log(message) {
    var text = "# ";
    if (true) {
        if ('undefined' !== typeof (message)) {
            try {
	        var json = JSON.stringify(message);
	        text += json;
            } catch (e) {
	        text += "not json: " + message;
            }
        }
        console.error(text);
    }
    return text;
}


function main()
{
    var client = require("iotivity-node").client;
    var Config = require('./artik-config-geolocation.js').Config;
    var sender = require('./artik-rest.js');
    var gThreshold = 200;
    var gGelocation = null;
    var gSwitch = null;

    function handle(properties)
    {
        log("handle: "+ "\n"
                    + JSON.stringify(properties , null, 4) + "\n"
                    + JSON.stringify(gGelocation , null, 4) + "\n"
                 );
        if (gThreshold > properties.illuminance) {
            if (gGelocation) {
                var data = { "lattitude": gGelocation.lat,
                             "longitude": gGelocation.lon,
                             "illuminance": properties.illuminance };
                log("alert: " + JSON.stringify(data));
                try {
                    if (true) 
                        sender.send(data, Config);
                } catch(err) {
                    log("error: sender");
                    log(err);
                };
            }
            if (gSwitch)
                console.log(1);
        } else {
            if (gSwitch)
                console.log(0);
        }
    }

    client.on("resourcefound", function(resource) {
        log("resourcefound: " + resource.resourcePath);

        if ("/IlluminanceResURI" === resource.resourcePath) {
            log(JSON.stringify(resource, null, 2));
	    var update = function(resource) {
                log("update: "  + resource.resourcePath + ": \n"
                             + JSON.stringify(resource.properties, null, 4));
                handle(resource.properties);
	    };
            resource.on("update", update);
        }
        else if ("/GeolocationResURI" === resource.resourcePath) {
            log(JSON.stringify(resource, null,2));
	    var update = function(resource) {
                log("update: "  + resource.resourcePath + ": \n"
                             + JSON.stringify(resource.properties, null, 4));
                gGelocation = resource.properties;
	    };
            resource.on("update", update);
        }
        else if ("/BinarySwitchResURI" === resource.resourcePath) {
            log(JSON.stringify(resource, null,2));
            gSwitch = resource;
        }
    });

    client.findResources().catch(function(error) {
	console.error(error.stack ? error.stack 
                       : (error.message ? error.message : error));
	process.exit(1);
    });
}


main();

