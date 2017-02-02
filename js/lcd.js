#!/usr/bin/env node
//  while true ; do date +%s ; sleep 1 ; done  | node lcd.js 

var Lcd = require('lcd');


var line =  0;
var delay = 400;
var interval = null;


function exit()
{
     console.log("exit:");
     setTimeout(function(){
		lcd.close();
		process.exit()}
		, delay);
}



function print(str,pos)
{
    if ( undefined === pos ) {
        pos = 0;
        lcd.clear();
        lcd.setCursor(0,0);
    }
    
    lcd.print(str, 
              function (err) { if (err) { throw err; } });
}


function read()
{
    console.log("read");
    var readline = require('readline');

    var reader = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
        terminal: false
    });
    
    reader.on('line', function (input) {
        console.log("output: " + input );
        print(input);
    });

    reader.on('pause', () => { console.log("pause"); } );

   reader.on('resume', () => { console.log('Readline resumed.');});

    reader.on('close', exit, delay);
    console.log("read");
}


function readIn()
{
  process.stdin.setEncoding('utf8')
  process.stdin.resume()
  process.stdin.on('data', function (command) {
    console.log('got command: %s', command.trim())
    print(command);
  })
}

exports.main = function()
{
    console.log("main");
    lcd = new Lcd({rs: 4, e: 21, data: [27, 22, 10, 9], cols: 16, rows: 1});
    lcd.on('ready', function () {
        readIn();
    });

    process.on('SIGINT',exit);
    console.log("main");
}


if (require.main === module) {
    exports.main();
}
