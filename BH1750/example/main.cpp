//	照度センサー BH1750 (モジュール名 GY-302) ライブラリ
//	『昼夜逆転』工作室	http://jsdiy.starfree.jp
//	2025/01	@jsdiy

#include	<Arduino.h>
#include	<Wire.h>
#include	"BH1750.hpp"

BH1750	sensor;

void	setup(void)
{
	Serial.begin(115200);
	Wire.begin();
	sensor.Initialize(ERunning::OneTime, EResolution::HighResMode);
}

void	loop(void)
{
	sensor.PowerOn();
	uint16_t lux = sensor.Lux();
	Serial.printf("illuminance: %d(Lux)\n", lux);
	delay(1000 - sensor.MeasurementTime());
}
