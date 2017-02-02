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

// npm install bh1750
// ls /dev/i2c*
// i2cdetect -y 1
var device = '/dev/i2c-1';
var address = 0x23;
var delay = 1000;
var options =
    {
        address: address,
        device: device,
        command: 0x10,
        length: 2
    };

var EventEmitter = require('events').EventEmitter;
var interval = null;
var BH1750 = require('bh1750');

function Updater(name)
{
    EventEmitter.call(this);
    Updater.prototype.__proto__ = EventEmitter.prototype;

    var self = this;    
    self.name = name;
    self.member = new BH1750( { options } );
    self.inverval = null;
    self.start = function() {
        self.interval = interval = setInterval(function(){

            self.member.readLight(function(value){
                var value = { "illuminance" : value };
                self.emit("update", value);
            });
        }, delay);
    }
    
    self.stop = function() {
        clearInterval( self.interval );
        self.interval = null;
    }

    self.main = function main()
    {
        self.on('update', console.log);
        self.start();
        self.on('update', function(value) {
            //this.emit("update", value );
        } );
    }

}


exports.instance = new Updater("BH1750");
exports.start = exports.instance.start;
exports.main = exports.instance.main;


if (require.main === module) {
    exports.main();
}
