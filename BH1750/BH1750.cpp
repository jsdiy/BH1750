//	照度センサー BH1750 (モジュール名 GY-302) ライブラリ
//	『昼夜逆転』工作室	http://jsdiy.starfree.jp
//	2025/01	@jsdiy

#include	<Arduino.h>
#include	<Wire.h>
#include	"BH1750.hpp"

//命令を送信する
void	BH1750::WriteByte(EInstruction inst)
{
	Wire.beginTransmission(slaveAddress);
	Wire.write(inst);
	Wire.endTransmission();
}

//測定値を読み込む
//参考：BH1750はHiByte-LoByteの順に値が読み出される。
uint16_t	BH1750::ReadWord(void)
{
	Wire.beginTransmission(slaveAddress);
	Wire.endTransmission();

	Wire.requestFrom(slaveAddress, (size_t)2);
	uint8_t	hiByte = Wire.read();
	uint8_t	loByte = Wire.read();
	uint16_t	data = (hiByte << 8) | loByte;
	return data;
}

//初期化
void	BH1750::Initialize(ERunning running, EResolution resolution)
{
	SetCondition(running, resolution);
	PowerOn();
	WriteByte(EInstruction::Reset);
	PowerDown();
}

//測定条件をセットする
void	BH1750::SetCondition(ERunning running, EResolution resolution)
{
	measInst = static_cast<EInstruction>(running | resolution);

	switch (resolution)
	{
	default:
	case EResolution::HighResMode:	measTime = EMeasurementTime::HighResModeMSec;	break;
	case EResolution::HighResMode2:	measTime = EMeasurementTime::HighResMode2MSec;	break;
	case EResolution::LowResMode:	measTime = EMeasurementTime::LowResModeMSec;	break;
	}
}

//照度を取得する（HighResMode, LowResMode用）	※戻り値は測定値ではなく照度
//・照度 = 測定値 / 1.2 [lux]
uint16_t	BH1750::Lux(void)
{
	WriteByte(measInst);
	delay(measTime);
	uint32_t	data = ReadWord();
	uint16_t	lux = static_cast<uint16_t>(data * 10 / 12);
	return lux;
}

//照度を取得する（HighResMode2用）	※戻り値は測定値ではなく照度
//・ReadWord()で取得する値は 0.0～32767.5(=0xFFFF) を表す。これを1.2で割った値が照度。
//・Lux()の結果を2で割っても算数的には同じだが、整数に丸められた値で計算を進めたらHighResMode2で測定した意味がなくなる。
//・これらを考慮しながら整数型だけで演算するのは面倒くさいので素直にfloat型を使う。
String	BH1750::Lux2(void)
{
	WriteByte(measInst);
	delay(measTime);
	uint16_t	data = ReadWord();
	float	fLux = static_cast<float>(data) / 2.0f / 1.2f;
	return String(fLux);	//小数第2位までが文字列に変換される
}
