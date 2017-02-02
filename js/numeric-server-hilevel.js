#!/usr/bin/env node
var path = "/NumericResURI";
var sampleResourceType = "oic.r.switch.binary";

var myResource, device,
	_ = require( "lodash" ),
	observerCount = 0;

device = require("iotivity-node");
_.extend( device.device, {
	coreSpecVersion: "res.1.1.0",
	dataModels: [ "something.1.1.0" ],
	name: sampleResourceType
} );

var value;

device.registerResource({
	id: { path: path },
	resourceTypes: [ "oic.r.value" ],
	interfaces: [ "oic.if.baseline" ],
	discoverable: true,
	properties: { value: 0}
}).then(function(resource) {
	console.log("value: register OK");
	value = resource;
	device.on("retrieverequest", onGet);
});

function onGet(request) {
	console.log("value: request " + request.type);
	value.properties.value = 1;
	request.sendResponse(value);
}
