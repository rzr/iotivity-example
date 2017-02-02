#// # npm install ws
#//https://my.artik.cloud/

cat<<EOF>ws.js && node ws.js

var deviceId = "6b71b5fb2cf845f192ae72cf4298fcd7";
var deviceTypeId = "dt14f2e12d241a4b8f945e01b2f0db4473";
var token = "126fd4431cdc4b069b1dbf3d99e0f422";


var webSocketUrl = "wss://api.artik.cloud/v1.1/websocket?ack=true";
var WebSocket = require('ws');

function getTimeMillis()
{
    return new Date().getTime();
}

var cid = getTimeMillis();


function action(value)
{
    try{
        ts = ', "ts": '+getTimeMillis();
        var data = { "value": value  };
        var payload = '{"sdid":"'+deviceId+'"'+ts+', "data": '+JSON.stringify(data)+', "cid":"'+getTimeMillis()+'"}';
        console.log('Sending payload ' + payload);
        ws.send(payload, {mask: true});
    } catch (e) {
        console.error('Error in sending a message: ' + e.toString());
    }
}

function input(destination)
{
    console.log( "input:" + destination);
    process.stdin.resume();
    process.stdin.setEncoding('utf8');
    var value = 0;
    process.stdin.on('data', function (input) {
        value = parseInt(input.trim(), 10);
        action(value, destination);
    });
}


function register()
{
    console.log("Registering device on the websocket connection");
    try{
        var registerMessage = '{"type":"register", "sdid":"' + deviceId + '", "Authorization":"bearer ' + token + '", "cid":"' + cid + '"}' ;
        console.log('Sending register message ' + registerMessage + '\n');
        ws.send(registerMessage, {mask: true});
        isWebSocketReady = true;
        input();
    }
    catch (e) {
        console.error('Failed to register messages. Error in registering message: ' + e.toString());
    }
}


function start() {
    isWebSocketReady = false;
    ws = new WebSocket(webSocketUrl);
    ws.on('open', function() {
         console.log("Websocket connection is open ....");
         register();
    });
    ws.on('message', function(data, flags) {
         console.log("Received message: " + data + '\n');
    });
    ws.on('close', function() {
         console.log("Websocket connection is closed ....");
    });
}

start();


EOF

# https://developer.artik.cloud/dashboard/devicetype/dt14f2e12d241a4b8f945e01b2f0db4473/newManifest

# http://www.oneiota.org/revisions/2088  oic.r.airqualitycollection.json
