#!/usr/bin/env node
// Copyright 2016 Intel Corporation
// Copyright 2016 Samsung Electronics France SAS
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

//# /tmp/tmp-philippe.tmp/tmp/var/lib/disk.sh.dir/tmp/mnt/uri/ssh/root/eat.lan/usr/lib/node_modules/iotivity-node/iotivity-example/js/

function main()
{
    var client = require( "iotivity-node" ).client;
    var Config = require('./artik-config-binary.js').Config;
    var sender = require('./artik-rest.js');

    var gThreshold = 7;
    var gGelocation = null;

    function handle(properties)
    {
        console.log("handle: "+ "\n"
                    + JSON.stringify( properties, null, 4) + "\n"
                   );

        if (!true) sender.send(properties, Config);
    }
    
    client.on( "resourcefound", function( resource ) {
        console.log( "resourcefound: " + resource.resourcePath);
        
        if ( "/BinarySwitchResURI" === resource.resourcePath ){
            console.log( JSON.stringify( resource, null,2 ));
	    var update = function( resource ) {
                console.log( "update: "  + resource.resourcePath + ": \n"
                             + JSON.stringify( resource.properties, null, 4 ) );
                handle(resource.properties);
	    };
            resource.on( "update", update );
        }
    } );

    client.findResources().catch( function( error ) {
	console.error( error.stack ? error.stack 
                       : ( error.message ? error.message : error ) );
	process.exit( 1 );
    } );
}

main();
