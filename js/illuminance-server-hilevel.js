#!/usr/bin/env node
var resourcePath = "/IlluminanceResURI";
var resourceType = "oic.r.sensor.illuminance";

var myResource, device,
_ = require( "lodash" ),
observerCount = 0;

var gLogEnabled = false;

function log( object )
{
    if ( gLogEnabled )
	console.log("log: " + object );
}

function handleError( theError ) {
    console.error( theError );
    process.exit( 1 );
}


device = require("iotivity-node");
_.extend( device.device, {
    coreSpecVersion: "res.1.1.0",
    dataModels: [ "something.1.1.0" ],
    name: resourceType
} );
_.extend( device.platform, {
    manufacturerName: "Example",
    manufactureDate: new Date( "Tue Dec 20 14:48:07 CET 2016" ),
    platformVersion: "1.1.1",
    firmwareVersion: "0.0.1",
    supportUrl: "https://wiki.tizen.org/wiki/User:Pcoval"
} );


var resourceRequestHandlers = {

    retrieve: function( request ) {
	log( "retrieve" );
	request.respond( request.target ).catch( handleError );
	if ( "observe" in request ) {
	    observerCount += Math.max( 0, request.observe ? 1 : -1 );
	}
    },
    create: function( request ) {
	log( "create" );
	request.respond( request.target ).catch( handleError );
	if ( "create" in request ) {
	    observerCount += Math.max( 0, request.observe ? 1 : -1 );
	}
    },
    update: function( request ) {
	log( "update" );
	console.log( request.data.value ? "1" : "0" );
	request.target.properties.illuminance = gIlluminance;

	request.respond( request.target ).catch( handleError );
	if ( "update" in request ) {
	    observerCount += Math.max( 0, request.observe ? 1 : -1 );
	}
    },

};

if ( device.device.uuid ) {
    
    device.server.register( {
	resourcePath: resourcePath,
	resourceTypes: [ resourceType ],
	interfaces: [ "oic.if.baseline" ],
	discoverable: true,
	observable: true,
	properties: { illuminance: 0 }
    } ).then(
	function( resource ) {
	    log( "OCF resource successfully registered" );
	    myResource = resource;
            
	    log( " Add event handlers for each supported request type" );
	    _.each( resourceRequestHandlers, function( callback, requestType ) {
		log( "each: " + requestType );
		resource[ "on" + requestType ]( function( request ) {
		    log( "Received request " + JSON.stringify( request, null, 4 ) );
		    callback( request );
		} );
	    } );
	},
	function( error ) {
	    throw error;
	} );
}

var illuminance = new Date().valueOf();

device.server.register({
    id: { path: resourcePath },
    resourceTypes: [  resourceType ],
    interfaces: [ "oic.if.baseline" ],
    discoverable: true,
    observable: true,
    properties: { illuminance: illuminance  }
}).then(function(resource) {
    console.log("value: register OK");
    value = resource;
    device.on("retrieverequest", onGet);
});

function onGet(request)
{
    console.log("value: request " + request.type);
    value.properties.value = 1;
    request.sendResponse(value);
}

var delay = 1000;

function start() {
    var interval = setInterval(function(){
        gIlluminance = new Date().valueOf();
    }, delay);
}

start();

