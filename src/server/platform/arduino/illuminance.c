#include <Wire.h>

byte buff[2];


void BH1750_Init(int address)
{
    Wire.beginTransmission(address);
    Wire.write(0x10);
    Wire.endTransmission();
}


byte BH1750_Read(int address)
{
    byte i=0;
    Wire.beginTransmission(address);
    Wire.requestFrom(address, 2);
    while(Wire.available()){
        buff[i] = Wire.read(); 
        i++;
    }
    Wire.endTransmission();  
    return i;
}


int platform_getValue()
{
    float value=0;
    char status=0;
    static int once=1;
    static int const i2c_addr = 0x23;
    if (once) { 
        once--;
        Wire.begin();
        BH1750_Init(i2c_addr);
    }
    
    if(BH1750_Read(i2c_addr)==2){
        value=((buff[0]<<8)|buff[1])/1.2;
    }

    return (int) value;
}
