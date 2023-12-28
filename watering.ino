
#define SOIL_PIN A0

static const int VERY_WET = 350;
static const int VERY_DRY = 670;
static const int AVG = (VERY_WET + VERY_DRY) / 2;

static const int MIN_PUMP_TIME = 1000;
static const int MAX_PUMP_TIME = 5000;

static unsigned int pumpStartMilis = 0;
static bool pumpOn = false;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	int waterValue = analogRead(SOIL_PIN);
	Serial.println(waterValue);

	if (waterValue > AVG)
	{
		// Turn on the pump

		turnPumpOn();
	}
	else
	{
		// Turn off the pump

		turnPumpOff();
	}

	delay(500);
}

void turnPumpOff()
{
	Serial.println("Pump off");
	pumpOn = false;
}

void turnPumpOn()
{
	Serial.println("Pump on");
	pumpOn = true;
}