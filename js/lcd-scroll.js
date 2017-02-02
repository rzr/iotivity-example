var Lcd = require('lcd'),
lcd = new Lcd({rs: 4, e: 21, data: [27, 22, 10, 9], cols: 16, rows: 1,
              });

var line =  0;
var delay = 400;
var interval = null;

function print(str, pos) {
    console.log("print: " + pos + ":" + str);
    if ( pos === undefined ) {
        console.log("clear:");
        if ( interval != null) {
            clearTimeout(interval); 
            interval = null; 
        }
        lcd.autoscroll();
        lcd.clear(function(){
            line=(line+1) %lcd.rows;
            lcd.setCursor(lcd.cols, line);
            print(str,0);
        });
        return ;
    }

    pos = pos || 0;
    
    if (pos === str.length) {
        pos = 0;
        return ;
    }
    
    lcd.print(str[pos], function (err) {
        if (err) {
            throw err;
        }
        interval = setTimeout(function () {
            print(str, pos + 1);
        }, delay);
    });
}


function loop()
{
    var readline = require('readline');

    var iface = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
        terminal: false
    });
    console.log("type in stdin:");
    
    iface.on('line', function (input) {
        console.log("output: " + input );
        interval = setTimeout(function () {
            print(input + ' * ');
            
        }, delay * lcd.cols );

    });
}

exports.main = function()
{
    lcd.on('ready', function () {
        loop();
    });

    // If ctrl+c is hit, free resources and exit.
    process.on('SIGINT', function () {
        lcd.close();
        process.exit();
    });

}

if (require.main === module) {
    exports.main();
}
