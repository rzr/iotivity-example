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
    var resourcePath = "/IlluminanceResURI";
    var resourceType = "oic.r.sensor.illuminance";
    var gValues = { illuminance: 0 };

    iotivity = require( "iotivity-node/lowlevel" );

    var source = require("./bh1750.js").instance;

    var intervalId, handleReceptacle = {}, observerIds = [];

    iotivity.OCInit( null, 0, iotivity.OCMode.OC_SERVER );

    function notify(values)
    {
        console.log( "notify: observers=" 
                     +( observerIds.length > 0 ? 
                        + observerIds.length 
                        : 0));

        if ( observerIds.length > 0 ) {
	    iotivity.OCNotifyListOfObservers(
	        handleReceptacle.handle,
	        observerIds,
	        {
		    type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
		    values: values
	        },
	        iotivity.OCQualityOfService.OC_HIGH_QOS 
            );
        }
    }

    function handleEntity( flag, request )
    {
	console.log( "Entity handler called with flag = " + flag + " and the following request:" );
	console.log( JSON.stringify( request, null, 4 ) );
        
	if ( flag & iotivity.OCEntityHandlerFlag.OC_OBSERVE_FLAG ) {
	    if ( request.obsInfo.obsId !== 0 ) {
		if ( iotivity.OCObserveAction.OC_OBSERVE_REGISTER
                     === request.obsInfo.action ) {
                    observerIds.push( request.obsInfo.obsId );
		} else if ( iotivity.OCObserveAction.OC_OBSERVE_DEREGISTER 
                            === request.obsInfo.action ) {
                    observerIdIndex = observerIds.indexOf( request.obsInfo.obsId );
		    if ( observerIdIndex >= 0 ) {
			observerIds.splice( observerIdIndex, 1 );
		    }
		}
	    }
	}

	if ( request ) {
            console.log( JSON.stringify( request) );
	    iotivity.OCDoResponse( {
		requestHandle: request.requestHandle,
		resourceHandle: request.resource,
		ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
		payload: {
		    type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
		    values: gValues
		},
		resourceUri: resourcePath,
		sendVendorSpecificHeaderOptions: []
	    } );
            
	    return iotivity.OCEntityHandlerResult.OC_EH_OK;
	}
        
	return iotivity.OCEntityHandlerResult.OC_EH_ERROR;
    }

    iotivity.OCCreateResource(
        handleReceptacle,
        resourceType,
        iotivity.OC_RSRVD_INTERFACE_DEFAULT,
        resourcePath,
        handleEntity,
        iotivity.OCResourceProperty.OC_DISCOVERABLE |
            iotivity.OCResourceProperty.OC_OBSERVABLE
    );

    process.on( "SIGINT", function() {
        clearInterval( intervalId );
        iotivity.OCDeleteResource( handleReceptacle.handle );
        iotivity.OCStop();
        process.exit( 0 );
    } );

    source.start();
    source.on("update", function update(values){
        if ( gValues.illuminance != values.illuminance) {
            gValues = values;
            notify(gValues);
        }
    });

    intervalId = setInterval( function() {
        iotivity.OCProcess();
    }, 1000 );
}


if (require.main === module) {
    exports.main();
}
