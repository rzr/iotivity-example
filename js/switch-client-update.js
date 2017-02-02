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


function main()
{
    var client = require( "iotivity-node" ).client;
    var gSwitch = false;
    var value = !false;

    function handle(properties)
    {
        console.log("handle: "+ "\n"
                   );
        {
            var switchResource = {
                "resourceUri": "/BinarySwitchResURI",
                "properties": { "value": !value  }
            };


            switchResource = 
{
    "devAddr": {
        "adapter": 1,
        "flags": 64,
        "ifindex": 0,
        "port": 55555,
        "addr": "192.168.100.167"
    },
    "addr": {
        "adapter": 1,
        "flags": 64,
        "ifindex": 0,
        "port": 55555,
        "addr": "192.168.100.167"
    },
    "payload": {
        "type": 4,
        "values": {
            "value": false
        }
    },
    "connType": 65600,
    "identity": [],
    "result": 4,
    "sequenceNumber": 16777216,
    "resourceUri": "/BinarySwitchResURI",
    "rcvdVendorSpecificHeaderOptions": []
};



            client.update(switchResource);
        }
    }

    client.on( "resourcefound", function( resource ) {
        console.log( "resourcefound: " + resource.resourcePath);

        if ( "/BinarySwitchResURI" === resource.resourcePath ){
            console.log( JSON.stringify( resource, null,2 ));
	    var update = function( resource ) {
                console.log( "update: "  + resource.resourcePath + ": \n"
                             + JSON.stringify( resource.properties, null, 4 ) );
                //handle(resource.properties);
                //gSwitch = resource.properties.value;
	    };
            //resource.on( "update", update );

            handle();
        }
    } );

    client.findResources().catch( function( error ) {
	console.error( error.stack ? error.stack 
                       : ( error.message ? error.message : error ) );
	process.exit( 1 );
    } );
}

function input(destination)
{
    console.log( "input:" + destination);
    process.stdin.resume();
    process.stdin.setEncoding('utf8');
    var value = 0;
    process.stdin.on('data', function (input) {
        value = parseInt(input.trim(), 10);
        post(value, destination);
    });
}




main();

