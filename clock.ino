#include <stdio.h> // for function sprintf

int distance_sensor_pin = 2; //analog pin 0
int minute_increment_pin = 1;
int hour_increment_pin = 0;

bool minute_increment_pin_wasdown = false, hour_increment_pin_wasdown = false;

unsigned long last_tick = 0;

int second = 0, minute = 0, hour = 0;
int minute_digit = 0, minute_tens = 0, hour_digit = 0, hour_tens = 0;
int num_leds[4] = {4, 3, 4, 2}; //minute digit, minute tens, hour digit, hour tens

unsigned long time_active = 0;
unsigned long last_motion_time = 0;

bool active = false;

void setup(){
  //Serial.begin(9600);
  Serial.println("Init");

  for (int k = 0; k <= 13; k++){
    pinMode(k, OUTPUT);
    digitalWrite(k, LOW);    
  }

}

void loop(){
  float distance = 12343.85*pow(analogRead(distance_sensor_pin), -1.15);

  if (distance < 10){
    active = true;
    last_motion_time = millis();    
  } else {
    if (millis() - last_motion_time > 15000){
      for (int k = 0; k <= 13; k++){
        digitalWrite(k, LOW);
      }
      active = false;
    }
  }

  if (!active)
    return;
  
  /*******handle time incrementing with buttons********/
  if (analogRead(minute_increment_pin) < 100){
    minute_increment_pin_wasdown = true;
  } 

  if (analogRead(minute_increment_pin) == 1023 && minute_increment_pin_wasdown){ //1023 is max value (not pressed)
    minute++;
    minute_increment_pin_wasdown = false;
    second = 0;
    Serial.println("Button to increment minute");
  }

  if (analogRead(hour_increment_pin) < 100){
    hour_increment_pin_wasdown = true;
  } 

  if (analogRead(hour_increment_pin) == 1023 && hour_increment_pin_wasdown){
    hour++;
    hour_increment_pin_wasdown = false;
    second = 0;
    Serial.println("Button to increment hour");
  }
  
  /*******handle time updates********/
 
  if (millis() - last_tick >= 1000){
    char bff[20] = {};
    sprintf(bff, "H:%d M:%d: S:%d", hour, minute, second);
    Serial.println(bff);

    second += 1;
    last_tick = millis();
  }
  
  if (second >= 60){
    second = 0;
    minute++;
  }
  if (minute >= 60){
    minute = 0;
    hour++;
  }
  hour %=24;

  /*******convert to binary and control LEDs********/
  int vals_per_loc[4] = {minute%10, minute/10, hour%10, hour/10}; //minute digit, minute tens, hour digit, hour tens

  int led_output_pin = 0;
  for (int k = 0; k < 4; k++){ //loop per LED group from low to high
    for (int j = 0; j < num_leds[k]; j++){
      digitalWrite(led_output_pin++, (vals_per_loc[k] % 2? HIGH:LOW));
      vals_per_loc[k] /= 2;
    }
  }
  digitalWrite(led_output_pin, second%2==0?LOW:HIGH);
  
  
}

