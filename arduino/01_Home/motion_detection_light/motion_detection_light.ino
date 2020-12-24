
#define ON_TIME	30 // seconds
#define PIN_LED 3  // TODO

bool isDark = false;
bool motionDet = false;
bool pir1Act = false;
bool pir2Act = false;

int onCnt = 0;
unsigned long time = 0;
unsigned long ms = 0;
unsigned long onTime = 0;


void setup (void)
{
  
}

void loop (void)
{
	unsigned long now;
	
	
	// run at each 1ms
	now = millis ();
	if (now == time) // 1ms not passed yet
	{
		return;
	}
	else
	{
		time = now;
		ms++;
	}
	
	if (ms == 1000) // 1 sec has passed
	{
		if (onTime != 0) // LED ON still required: count down ON time
		{
			onTime--;
		}
	}
	else 
	{
		ms = 0;
	}

	
	// get isDark from light level
	
	// get motion detection e.g.:
	
	motionDet = false; // assume false
	if (isDark)
	{
		if ((pir1Act == true) || (pir2Act == true))
		{
		motionDet = true;
		}
	}
	

	if (isDark) // dark detected
	{
		if (motionDet == true) // any motion detected
		{
			onTime = ON_TIME;
		}
	}
	else // enough ambiental light, switch OFF LEDs
	{
		onTime = 0;
	}
	
	// output LED status
	
	if (onTime == 0)
	{
		digitalWrite (PIN_LED, HIGH);
	}
	else
	{	
		digitalWrite (PIN_LED, LOW);
	}
	
}
