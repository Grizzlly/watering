
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

void setup()
{
	Serial.begin(9600);
	pinMode(pump_pin, OUTPUT);
	pinMode(pump_dir_pin, OUTPUT);
	pinMode(buz_pin, OUTPUT);
	pinMode(SOIL_PIN, INPUT);

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

	if (waterValue >= AVG)
	{
		// Turn on the pump
		if (!pumpOn)
		{
			turnPumpOn();
		}
		else if (millis() - pumpStartMilis > MAX_PUMP_TIME)
		{
			turnPumpOff();
			canPump = false;

			// for some reason the pump is not turning off
			// need device reset
			digitalWrite(buz_pin, HIGH);
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