
#if 0
#include <SFE_BMP180.h>
int platform_getValue()
{
    double value=0;
    char status=0;
    static SFE_BMP180 sensor;
    static int once=1;
    if (once) { once--;
        status = sensor.startTemperature();
    }
    status = sensor.getTemperature(value);
    return (int) value;
}
#else
int platform_getValue()
{
    double value=0;
    return (int) value;
}

#endif
