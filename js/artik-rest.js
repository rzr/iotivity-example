#!/usr/bin/env node
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

// Usage:
// # npm install node-rest-client

var Config = require("./artik-config-binary.js");
var url = "https://api.artik.cloud/v1.1/messages";

var Client = require("node-rest-client").Client;
var client = new Client();

function log(object)
{
    console.error(object);
}

function send(data, config)
{
    var ts = new Date().valueOf();
    var message = {
        headers: {
            "Content-Type": "application/json",
            "Authorization": "bearer " + config.token
        },
        data: {
            "sdid": config.deviceId,
            "ts": ts,
            "type": "message",
            "data": data,
        }
    };
    
    client.post(url, message, function(data, response) {
        log(data);
    }).on('error', log);
}


function main()
{

    var value = true;
    var delay = 5000;
    
    setInterval(function(){
        value = ! value;
        var data = { "value" : value };
        send(data);
    }, delay);
}


exports.send = send;
exports.main = main;
if (require.main === module) {
    exports.main();
}
