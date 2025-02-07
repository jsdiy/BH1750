//	照度センサー BH1750 (モジュール名 GY-302) ライブラリ
//	『昼夜逆転』工作室	http://jsdiy.starfree.jp
//	2025/01	@jsdiy

#pragma	once

#include	<Arduino.h>

//スレーブアドレス(7bit)	※どれか一つを有効、他はコメント化する
#define		BH1750_SLAVE_ADDRESS	0b0100011	//ADDR='L'	0x23	※デフォルト
//#define	BH1750_SLAVE_ADDRESS	0b1011100	//ADDR='H'	0x5C

//Instruction Set Architecture
//・OneTimeMode: It is automatically set to Power Down mode after measurement.
enum	EInstruction	: uint8_t
{
	PowerDown	= 0b00000000,	//No active state.
	PowerOn		= 0b00000001,	//Waiting for measurement command.
	Reset		= 0b00000111,	//Reset Data register value. Reset command is not acceptable in Power Down mode.
	ContinuouslyHighResMode		= 0b00010000,	//Start measurement at 1lx resolution. Measurement Time is typically 120ms.
	ContinuouslyHighResMode2	= 0b00010001,	//Start measurement at 0.5lx resolution. Measurement Time is typically 120ms.
	ContinuouslyLowResMode		= 0b00010011,	//Start measurement at 4lx resolution. Measurement Time is typically 16ms.
	OneTimeHighResMode	= 0b00100000,	//Start measurement at 1lx resolution. Measurement Time is typically 120ms.
	OneTimeHighResMode2	= 0b00100001,	//Start measurement at 0.5lx resolution. Measurement Time is typically 120ms.
	OneTimeLowResMode	= 0b00100011,	//Start measurement at 4lx resolution. Measurement Time is typically 16ms.
	//ChangeMeasurementTimeHighBit	= 0b01000_MT[7,6,5],	//Change measurement time.
	//ChangeMeasurementTimeLowBit	= 0b011_MT[4,3,2,1,0],	//Change measurement time.
	//・このライブラリではセンサー感度調整は非対応とする。
};
/*	Measurement mode explanation
Measurement Mode.	Measurement Time.	Resolurtion.
H-resolution Mode2.	Typ. 120ms.			0.5 lx.	//[15:1]が整数部、[0:0]が小数部を表す
H-Resolution Mode.	Typ. 120ms.			1 lx.	//[15:0]がそのままlux.
L-Resolution Mode.	Typ. 16ms.			4 lx.	//[15:0]のうち[1:0]は常に0b00
We recommend to use H-Resolution Mode.
*/

//モードの組合せ
enum	ERunning : uint8_t
{
	Continuously	= 0b00010000,
	OneTime			= 0b00100000,
};

//モードの組合せ
enum	EResolution : uint8_t
{
	HighResMode		= 0b00000000,
	HighResMode2	= 0b00000001,
	LowResMode		= 0b00000011,
};

//測定時間(ms)
//・120ms.typ at H-resolution mode, 16ms.typ at L-resolution mode.
//	ただしHighResMode2,LowResModeには別途注釈がある。
enum	EMeasurementTime : uint8_t
{
	HighResModeMSec		= 120 + 60,	//HighResMode2と同じだけ余分に待つこととする。
	HighResMode2MSec	= 120 + 60,	//Wait to complete 1st H-resolution mode2 measurement.( max. 180ms. )
	LowResModeMSec		= 16 + 8,	//Wait to complete L-resolution mode measurement.( max. 24ms. )
};

class	BH1750
{
private:
	const	uint8_t	slaveAddress = BH1750_SLAVE_ADDRESS;
	EInstruction	measInst;
	EMeasurementTime	measTime;
	void	WriteByte(EInstruction inst);
	uint16_t	ReadWord(void);
	void	SetCondition(ERunning running, EResolution resolution);

public:
	BH1750(void) {}
	void	Initialize(ERunning running = OneTime, EResolution resolution = HighResMode);
	void	PowerOn(void) { WriteByte(EInstruction::PowerOn); }
	void	PowerDown(void) { WriteByte(EInstruction::PowerDown); }	//OneTimeの場合は呼び出し不要（1回測定ごとに自動的にPowerDownされる）
	uint8_t	MeasurementTime(void) { return measTime; }
	uint16_t	Lux(void);	//HighResMode, LowResMode用
	String		Lux2(void);	//HighResMode2用
};
