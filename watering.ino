
#define SOIL_PIN A5

static const int VERY_WET = 290;
static const int VERY_DRY = 670;
static const int AVG = (VERY_WET + VERY_DRY) / 2;

static const int MIN_PUMP_TIME = 2000;
static const int MAX_PUMP_TIME = 7000;

static unsigned int pumpStartMilis = 0;
static bool pumpOn = false;
static bool canPump = true;

static const int buz_pin = 4;

static const int pump_pin = 10;
static const int pump_pwm_val = 200;
static const int pump_dir_pin = 12;

static const int flowSensorPin = 2;
static volatile byte pulseCount = 0;
static unsigned int oldPulseTimme = 0;
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
static const float calibrationFactor = 4.5;

void setup()
{
	Serial.begin(9600);
	pinMode(pump_pin, OUTPUT);
	pinMode(pump_dir_pin, OUTPUT);
	pinMode(buz_pin, OUTPUT);
	pinMode(SOIL_PIN, INPUT);

	pinMode(flowSensorPin, INPUT);
	digitalWrite(flowSensorPin, HIGH);
	attachInterrupt(digitalPinToInterrupt(flowSensorPin), flow, FALLING);

	digitalWrite(pump_dir_pin, HIGH);
}

void loop()
{
	delay(500);

	int waterValue = analogRead(SOIL_PIN);
	Serial.println(waterValue);

	if (!canPump)
	{
		return;
	}

	if (pumpOn && millis() - oldPulseTimme > 1000)
	{
		detachInterrupt(digitalPinToInterrupt(flowSensorPin));

		const float flowRate = ((1000.0 / (millis() - oldPulseTimme)) * pulseCount) / calibrationFactor;

		oldPulseTimme = millis();
		pulseCount = 0;

		if (flowRate < 0.5)
		{
			// pump is not working
			throwError();
			return;
		}
		else
		{
			Serial.print("Flow rate: ");
			Serial.print(int(flowRate));
			Serial.print(" L/min");
			Serial.println();
		}

		attachInterrupt(digitalPinToInterrupt(flowSensorPin), flow, FALLING);
	}

	if (waterValue >= AVG)
	{
		// Turn on the pump
		if (!pumpOn)
		{
			turnPumpOn();
		}
		else if (millis() - pumpStartMilis > MAX_PUMP_TIME)
		{
			// for some reason the pump is not turning off
			// need device reset
			throwError();
			return;
		}
	}
	else
	{
		// Turn off the pump
		if (pumpOn && millis() - pumpStartMilis > MIN_PUMP_TIME)
		{
			turnPumpOff();
		}
	}
}

void turnPumpOff()
{
	Serial.println("Pump off");
	pumpOn = false;
	analogWrite(pump_pin, 0);
	pumpStartMilis = 0;
}

void turnPumpOn()
{
	Serial.println("Pump on");
	pumpOn = true;
	analogWrite(pump_pin, pump_pwm_val);
	pumpStartMilis = millis();
}

void throwError()
{
	turnPumpOff();
	canPump = false;
	digitalWrite(buz_pin, HIGH);
}

void flow()
{
	pulseCount++;
}