// # npm install ws
//https://my.artik.cloud/
// "X-Quota-Daily-Messages":"150/150"


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
var gAlert = false;
var isWebSocketReady = false;
var ws = null;

function send(value)
{
    if ( null == ws ) { start();  return; }
    try{
        var data = { "value": value  };
        var now = getTimeMillis();
        var message= { "sdid": deviceId, "ts": now, "data": data, "cid": now };
        console.log(JSON.stringify(message));
        ws.send(JSON.stringify(message), {mask: true});
    } catch (e) {
        console.error('error: on send: ' + e.toString());
    }
}

function register()
{
    try{
        var message = { "type":"register", "sdid": deviceId , "Authorization":"bearer",  token, "cid": + cid };
        ws.send(JSON.stringify(message), {mask: true});
        isWebSocketReady = true;
        input();
    }
    catch (e) {
        console.error('error: on register: ' + e.toString());
    }
}


function start()
{
    if ( isWebSocketReady) { return; }
    isWebSocketReady = false;
    ws = new WebSocket(webSocketUrl);
    ws.on('open', function() {
        register();
    });
    ws.on('message', function(data, flags) {
        console.log("message: " + data + '\n');
    });
    ws.on('close', function() {
        console.log("close: ");
    });
}

// for testing purpose
function input()
{
    console.log( "input:");
    process.stdin.resume();
    process.stdin.setEncoding('utf8');
    var value = 0;
    process.stdin.on('data', function (input) {
        value = parseInt(input.trim(), 10);
        send(value);
    });
}

// for testing purpose
function main()
{
    input();
}

exports.send = send;

exports.main = main;

if (require.main === module) {
    exports.main();
}
