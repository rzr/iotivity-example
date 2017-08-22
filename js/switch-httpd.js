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

var port = 8080;
var delay = 3000;
var url = "http://localhost:" + port +"/#";

var fs = require('fs');
var http = require('http');

var ReadableStream = Object.getPrototypeOf(process.stdin);

var state=0;

function log(message)
{
    var text = "log: " + message;
    // var text = JSON.stringify(message)   
    if (!true)
	console.log(text);
}


function sendEvent(connection, text)
{
    log("sendEvent");
    if (null == text) {
        text = "" + new Date();
    }
    connection.send(text);
    log(text);
}


function start(connection)
{
    log("start");
    // var id = setInterval(function() { sendEvent(connection); } , delay);
    //TODO: clearInterval(id);
    process.stdin.resume();
    process.stdin.setEncoding('utf8');
    process.stdin.on('data', function(data) {
        log(data);
        sendEvent(connection, data);
    });

}


function toggle()
{
	state=(!state);
	message = (state)?"1":"0";
	console.log(message);
}


function handleRequest(request, response){
    var message="";
    log("request: " + message);
    
    // https://nodejs.org/api/http.html#http_class_http_incomingmessage
    var t = require('url').parse(request.url);

    if (t != undefined) { 
	t = t.query; 
    }
    log("t=" + t);
    
    if (t != undefined) { 

	message = "response: " + (Date()) ;
	message += " ";
	message += request.url;
	response.end(message);
	
	toggle();
    }
}


log(process.argv)
var server = http.createServer(handleRequest);

server.listen(port , function() {
    log((new Date()) + url);
});
