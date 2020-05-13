#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <math.h>

float menuLoadValueHandler();
void menuHandler();
void menuDisplayInfo0Handler();
void menuDisplayInfor1Handler();
int menuSaveHandler();
float voltageReadingToCelsius(int reading);
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void loadValuesFromEeprom();


LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

int upPin = 1;
int downPin = 2;
int leftPin = 0;
int rightPin = 3;
int selectPin = 4;
int cancelPin = 5;

int thinknessPin = A5;
int motorPin = 6;
int heater1Pin = A2;
int heater2Pin = A3;
int temperature1Pin = A0;
int temperature2Pin = A1;

int start = 0;
float temperature1current = 0;
float temperature2current = 0;
float temperature1target = 100;
float temperature2target = 50;

float motorSpeed = 0;
float motorSpeedStep = 0.5;

float thicknessTarget = 1.75;
float thicknessCurrent = 0;

int temperature1Reached = 0; 
int temperature2Reached = 0;
int heater1On = 0;
int heater2On = 0;

int menuSelected = 0;
int menuEditMode = 0;
float menuEditValueTemp = 0;
float menuEditValueStep = 0.5;

int savedValuesToEepromAddress = 0;
int temperature1targetAddress = 1;
int temperature2targetAddress = 5;
int motorSpeedStepAddress = 9;
int thicknessTargetAddress = 13;

char* menuItems[]={
  	"Temp Info", 		//not displayed
  	"Thicknes Info",	//not displayed
  	"Start",			//displayed but value is set in function
    "Set Temp 1", 
    "Set Temp 2", 
  	"Set Thickness",
  	"Set Motor Step"
  };
int menuItemsCount = 6;
//int menuItemsCount = sizeof(menuItems) / sizeof(menuItems[0]) - 1;


float menuLoadValueHandler(){
  //sync these values with the menu array index
  if(menuSelected == 3){ return temperature1target; }
  else if(menuSelected == 4){ return temperature2target; }
  else if(menuSelected == 5){ return thicknessTarget; }
  else if(menuSelected == 6){ return motorSpeedStep; }
}

void menuHandler(){
  	if(digitalRead(leftPin) == LOW){ menuSelected--; }
  	else if(digitalRead(rightPin) == LOW){ menuSelected++; }
  
  	if(menuSelected < 0){ 
      	menuSelected = menuItemsCount; 
    }else if(menuSelected > menuItemsCount){ 
      	menuSelected = 0; 
    }
  
  	lcd.clear();
  	if(menuSelected == 0){
    	menuDisplayInfo0Handler();
    }else if(menuSelected == 1){
       	menuDisplayInfor1Handler();
    }else if(menuSelected == 2){
    	if(digitalRead(selectPin) == LOW){
          start = !start;
        }
      	
      	if(start){
        	lcd.write("Stop");
        }else{
        	lcd.write("Start");
        }
    }else{
      if(digitalRead(selectPin) == LOW){
        //save
        if(menuEditMode){
			menuSaveHandler();
          	menuEditMode = 0;
        //load
        }else{
          	menuEditMode = 1;
          	menuEditValueTemp = menuLoadValueHandler();
        }
      }
      
      //cancel
      if(digitalRead(cancelPin) == LOW){ 
        menuEditMode = 0;
        menuEditValueTemp = 0;
      }
      
      //edit
      if(menuEditMode){
        if(digitalRead(upPin) == LOW){ menuEditValueTemp += menuEditValueStep; }
        else if(digitalRead(downPin) == LOW){ menuEditValueTemp -= menuEditValueStep; }
        
        lcd.print(menuEditValueTemp);  
      }else{
      	lcd.print(menuItems[menuSelected]);
      }
    }
}


void menuDisplayInfo0Handler(){
	lcd.print("1:");
      lcd.print(round(temperature1current));
      lcd.print("/");
      lcd.print(round(temperature1target));
      lcd.print("c ");
      lcd.print("H:");
      lcd.print(heater1On);
      lcd.setCursor(0, 1);
      lcd.print("2:");
      lcd.print(round(temperature2current));
      lcd.print("/");
      lcd.print(round(temperature2target));
      lcd.print("c ");
      lcd.print("H:");
      lcd.print(heater2On);
}

void menuDisplayInfor1Handler(){
	lcd.print("Thi:");
     lcd.print(thicknessCurrent);
      lcd.print("/");
      lcd.print(thicknessTarget);
      lcd.print("mm");
      lcd.setCursor(0, 1);
      lcd.print("Motor speed:");
      lcd.print((int)motorSpeed);
      lcd.print("%");   
}

int menuSaveHandler(){
	if(menuSelected == 2){ 
    	temperature1target = menuEditValueTemp;
        EEPROM.write(temperature1targetAddress, temperature1target);
   	}else if(menuSelected == 3){ 
        temperature2target = menuEditValueTemp;
        EEPROM.write(temperature2targetAddress, temperature2target);
    }else if(menuSelected == 4){ 
        thicknessTarget = menuEditValueTemp;
        EEPROM.write(thicknessTargetAddress, thicknessTarget);
    }else if(menuSelected == 5){ 
        motorSpeedStep = menuEditValueTemp;
        EEPROM.write(motorSpeedStepAddress, motorSpeedStep);
    }
    EEPROM.write(savedValuesToEepromAddress, 1);
  	return 1;
}




float voltageReadingToCelsius(int reading){
 	float voltage = reading * 5.0 / 1024;
 	float temperatureC = (voltage - 0.5) * 100;
	return temperatureC;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loadValuesFromEeprom(){
  	unsigned char loadSavedValues = EEPROM.read(savedValuesToEepromAddress);
  	if(loadSavedValues == 0){ return; }
	temperature1target = EEPROM.read(temperature1targetAddress);
	temperature2target = EEPROM.read(temperature2targetAddress);
	motorSpeedStep = EEPROM.read(motorSpeedStepAddress);
	thicknessTarget = EEPROM.read(thicknessTargetAddress);
}

void setup() {
	pinMode(upPin, INPUT_PULLUP);
	pinMode(downPin, INPUT_PULLUP);
	pinMode(leftPin, INPUT_PULLUP);
	pinMode(rightPin, INPUT_PULLUP);
	pinMode(selectPin, INPUT_PULLUP);
	pinMode(cancelPin, INPUT_PULLUP);
  
	pinMode(thinknessPin, INPUT);
	pinMode(motorPin, OUTPUT);
	pinMode(heater1Pin, OUTPUT);
	pinMode(heater2Pin, OUTPUT);
	pinMode(temperature1Pin, INPUT);
	pinMode(temperature2Pin, INPUT);  
  
  	lcd.begin(16, 2);
  	lcd.cursor();
  	loadValuesFromEeprom();
}

void loop(){
	temperature1current = voltageReadingToCelsius(analogRead(temperature1Pin));
	temperature2current = voltageReadingToCelsius(analogRead(temperature2Pin));

  	int thicknessCurrentValue = analogRead(thinknessPin);
  	thicknessCurrent = mapfloat(thicknessCurrentValue, 0, 1024, 0, 5);

    if(start){
        temperature1Reached = temperature1current > temperature1target;
  		temperature2Reached = temperature2current > temperature2target;
  	
  		heater1On = !temperature1Reached;
  		heater2On = !temperature2Reached;
		
      	if(!temperature1Reached || !temperature2Reached){
      		motorSpeed -= motorSpeedStep;
    	}else{
      		if(thicknessCurrent > thicknessTarget){
         		motorSpeed += motorSpeedStep;
      		}else{
         		motorSpeed -= motorSpeedStep;
      		}
    	}
  		
      	if(motorSpeed > 100){ 
    		motorSpeed = 100; 
  		}else if(motorSpeed < 0){
  			motorSpeed = 0;
  		}
  
    }else{
    	heater1On = 0;
    	heater2On = 0;
    	motorSpeed = 0;
    }
  	
  	digitalWrite(heater1Pin, heater1On);
	digitalWrite(heater2Pin, heater2On);
  	analogWrite(motorPin, map(motorSpeed, 0, 100, 0, 255));
    
  	menuHandler();
    delay(100);
}