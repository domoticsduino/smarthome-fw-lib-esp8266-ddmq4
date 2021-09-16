/* 1.0.0 VERSION */

#include "ddmq4.h"

DDMQ4::DDMQ4(int pin)
{
	this->_analogPin = pin;
}

DDMQ4Val DDMQ4::getValue()
{

	float sensor_volt; //Define variable for sensor voltage
	float RS_gas;			 //Define variable for sensor resistance
	float ratio;			 //Define variable for ratio

	float sensorValue = analogRead(this->_analogPin);

	sensor_volt = sensorValue * (3.3 / 1023.0);		//Convert analog values to voltage
	RS_gas = ((3.3 * 10.0) / sensor_volt) - 10.0; //Get value of RS in a gas
	ratio = RS_gas / this->_R0;										// Get ratio RS_gas/RS_air

	double ppm_log = (log10(ratio) - this->_b) / this->_m; //Get ppm value in linear scale according to the the ratio value
	double ppm = pow(10, ppm_log);												 //Convert ppm value to log scale
	double percentage = ppm / 10000;											 //Convert to percentage

	float realValue = 0.0;

	if (sensorValue > this->_valorecentrale)
	{
		realValue = this->fscale(this->_valorecentrale, 1023, 1000, 10000, sensorValue, 10);
	}
	else
	{
		realValue = this->fscale(0, this->_valorecentrale, 200, 1000, sensorValue, 10);
	}

	DDMQ4Val ret;
	ret.realValue = realValue;
	ret.ppm = ppm;
	ret.sensorValue = sensorValue;
	ret.percentage = percentage;
	ret.success = true;

	return ret;
}

float DDMQ4::fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{

	float OriginalRange = 0;
	float NewRange = 0;
	float zeroRefCurVal = 0;
	float normalizedCurVal = 0;
	float rangedValue = 0;
	boolean invFlag = 0;

	// condition curve parameter
	// limit range

	if (curve > 10)
		curve = 10;
	if (curve < -10)
		curve = -10;

	curve = (curve * -.1);	// - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
	curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

	/*
    Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
    Serial.println(); 
    */

	// Check for out of range inputValues
	if (inputValue < originalMin)
	{
		inputValue = originalMin;
	}
	if (inputValue > originalMax)
	{
		inputValue = originalMax;
	}

	// Zero Refference the values
	OriginalRange = originalMax - originalMin;

	if (newEnd > newBegin)
	{
		NewRange = newEnd - newBegin;
	}
	else
	{
		NewRange = newBegin - newEnd;
		invFlag = 1;
	}

	zeroRefCurVal = inputValue - originalMin;
	normalizedCurVal = zeroRefCurVal / OriginalRange; // normalize to 0 - 1 float

	/*
    Serial.print(OriginalRange, DEC);  
    Serial.print("   ");  
    Serial.print(NewRange, DEC);  
    Serial.print("   ");  
    Serial.println(zeroRefCurVal, DEC);  
    Serial.println();  
    */

	// Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
	if (originalMin > originalMax)
	{
		return 0;
	}

	if (invFlag == 0)
	{
		rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;
	}
	else // invert the ranges
	{
		rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
	}

	return rangedValue;
}