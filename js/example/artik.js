// # npm install ws
// # https://developer.artik.cloud/dashboard/devicetypes
// # https://my.artik.cloud/devices
// # https://my.artik.cloud/data
// "X-Quota-Daily-Messages":"150/150"

var deviceId = "8602e3a2a73a430abc74c66c496bed72";
var deviceTypeId = "dtc75f653d9079424a89ed97aa98c9af3d";
var token = "99d0685f7d4147979bd8bcb6e2e5fa1d";
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

function send(data)
{
    if ( null == ws ) { start();  return; }
    try{
        var now = getTimeMillis();
        var message= { "data": data, 
                       "sdid": deviceId, "ts": now, "cid": now };
        
        console.log(JSON.stringify(message));
        ws.send(JSON.stringify(message), {mask: true});
    } catch (e) {
        console.error('error: on send: ' + e.toString());
    }
}

function register()
{
    try{
        var message = { "type":"register", "cid": cid,
                        "sdid": deviceId ,
                        "Authorization":"bearer "+ token };
        ws.send(JSON.stringify(message), {mask: true});
        isWebSocketReady = true;
        if ( false) input();
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
