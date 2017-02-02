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
var client = require( "iotivity-node" ).client;

var resourcePath = "/GeolocationResURI";

client.on( "resourcefound", function( resource ) {
    console.log( "resourcefound: " + resource.resourcePath);
    
    if ( resourcePath === resource.resourcePath ){
	var resourceUpdate = function( resource ) {
	    console.log( "properties: \n" 
                         + JSON.stringify( resource.properties, null, 4 ) );
	};
        resource.on( "update", resourceUpdate );
    }
} );


client.findResources().catch( function( error ) {
	console.error( error.stack ? error.stack 
                       : ( error.message ? error.message : error ) );
	process.exit( 1 );
} );
