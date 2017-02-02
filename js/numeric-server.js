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

exports.main = function()
{
    var sampleUri = "/NumericResURI";
    var sampleResourceType = "org.example.numeric";
    
    var gValue = 0; // TODO

    var intervalId,
    handleReceptacle = {},

    // This is the same value as server.get.js
    sampleUri = sampleUri,
    iotivity = require( "iotivity-node/lowlevel" );

    var source = require("./bh1750.js").instance;

    console.log( "Starting OCF stack in server mode : "  + sampleUri);

    // Start iotivity and set up the processing loop
    iotivity.OCInit( null, 0, iotivity.OCMode.OC_SERVER );

    iotivity.OCSetDeviceInfo( {
        specVersion: "res.1.0.0",
        dataModelVersions: [ "abc.0.0.1" ],
        deviceName: "server.get",
        types: []
    } );
    iotivity.OCSetPlatformInfo( {
        platformID: "server.get.sample",
        manufacturerName: "iotivity-node"
    } );


    console.log( "Registering resource" );

    var handleReceptacle = {}, observerIds = [];

    function notify(value)
    {
        console.log( "Sensor data has changed. " +
		     ( observerIds.length > 0 ?
		       "Notifying " + observerIds.length + " observers." :
		       "No observers in list." ) );
        if ( observerIds.length > 0 ) {
	    iotivity.OCNotifyListOfObservers(
	        handleReceptacle.handle,
	        observerIds,
	        {
		    type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
		    values: { "value": value } 
	        },
	        iotivity.OCQualityOfService.OC_HIGH_QOS );
        }
    }



    // Create a new resource
    iotivity.OCCreateResource(

        // The bindings fill in this object
        handleReceptacle,

        sampleResourceType,
        iotivity.OC_RSRVD_INTERFACE_DEFAULT,
        sampleUri,
        function( flag, request ) {
	    console.log( "Entity handler called with flag = " + flag + " and the following request:" );
	    console.log( JSON.stringify( request, null, 4 ) );

	    if ( flag & iotivity.OCEntityHandlerFlag.OC_OBSERVE_FLAG ) {
	        if ( request.obsInfo.obsId !== 0 ) {
		    if ( request.obsInfo.action === iotivity.OCObserveAction.OC_OBSERVE_REGISTER ) {
                        
		        // Add new observer to list.
		        observerIds.push( request.obsInfo.obsId );
		    } else if ( request.obsInfo.action ===
			        iotivity.OCObserveAction.OC_OBSERVE_DEREGISTER ) {
                        
		        // Remove requested observer from list.
		        observerIdIndex = observerIds.indexOf( request.obsInfo.obsId );
		        if ( observerIdIndex >= 0 ) {
			    observerIds.splice( observerIdIndex, 1 );
		        }
		    }
	        }
	    }

	    if ( request) {
	        iotivity.OCDoResponse( {
		    requestHandle: request.requestHandle,
		    resourceHandle: request.resource,
		    ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
		    payload: {
		        type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
		        values: {
                            "value" : gValue
		        }
		    },
		    resourceUri: sampleUri,
		    sendVendorSpecificHeaderOptions: []
	        } );

	        return iotivity.OCEntityHandlerResult.OC_EH_OK;
	    }

	    // By default we error out
	    return iotivity.OCEntityHandlerResult.OC_EH_ERROR;
        },
        iotivity.OCResourceProperty.OC_DISCOVERABLE |
            iotivity.OCResourceProperty.OC_OBSERVABLE
    );

    console.log( "Server ready" );

    // Exit gracefully when interrupted
    process.on( "SIGINT", function() {
        console.log( "SIGINT: Quitting..." );

        // Tear down the processing loop and stop iotivity
        clearInterval( intervalId );
        iotivity.OCDeleteResource( handleReceptacle.handle );
        iotivity.OCStop();

        // Exit
        process.exit( 0 );
    } );


    source.start();
    source.on("update", function update(value){
        console.log(value);
        if ( gValue != value.value )
            notify(value.value);
    });

    intervalId = setInterval( function() {
        iotivity.OCProcess();
    }, 1000 );
}


if (require.main === module) {
    exports.main();
}
