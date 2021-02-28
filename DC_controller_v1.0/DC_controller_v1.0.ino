//Arduino Uno based Simple PWM generation proportional to Mazda RX8 throttle pedal & LEM hass 300-s current sensor, 
//for use with and appropriate IGBT driver and power stage in DIY EV projects
//Experimental code. Only tested on the bench! Use at your own risk!
// S.Pilcher EV8



int pwm_pin = 6;  // 5v pwm to IGBT driver board
int pot_pin = A0; // input from throtle hall pedal 1-4v
int lem_pin = A1; // Input from LEM hass 300-s  2.5v + 0.0021v per amp messurement range 900amps
int brake_pin = 2; // input from brake pedal switch low=off 

int throttle_analog;
int pwm_throttle = 0;
int lem_value; // analog lem value
int pwm_percent;
int lem_amps; // current sensor mapped to AMPS
int pwm_duty = 0; 
int brake_state = LOW;


int r = 50; // pwm duty ramp increment.. (0-50) higher the number faster throttle ramps (set -9 for initial testing)
int s = 500; // soft cut current limit in AMPS
int h = 700; // hard cut cuttent limit in AMPS

void setup() {
  
  TCCR0B = TCCR0B & B11111000 | B00000010; // for PWM frequency of 7812.50 Hz
  pinMode(pwm_pin, OUTPUT);
  pinMode(brake_pin, INPUT);
  
  Serial.begin(9600); // intillalize serial port at baurd rate 9600
  
}

void loop() {
  // put you main code here, to run repeatedly:
  
  
  throttle_analog = analogRead(pot_pin);  //Reading from potentiometer
  lem_value = analogRead(lem_pin); //output from lem current sensor
  lem_amps = map(lem_value, 512, 899, 0,900); //Mapping lem_value as AMPS 
  lem_amps = constrain(lem_amps,0 ,1000);
  pwm_throttle = map(throttle_analog, 225, 680, 0, 254);  //Mapping the Values between 0 to 255 because we can give output  //from 0 -255 using the analogwrite funtion
  pwm_throttle = constrain(pwm_throttle, 0, 254); //Constaining values within pwm limits
  pwm_percent = map(pwm_duty, 0, 255, 0, 100); //Mapping pwm value as a percentage for serial print
  brake_state = digitalRead(brake_pin);
  
  
  
  
  
  if (pwm_throttle>pwm_duty) pwm_duty+=r;  // pwm duty increase/decrease with throttle 
  if (pwm_throttle<pwm_duty) pwm_duty-=r;
      
  if (pwm_throttle>pwm_duty) pwm_duty+=10;  
  if (pwm_throttle<pwm_duty) pwm_duty-=10;
      
  if (pwm_throttle>pwm_duty) pwm_duty++; 
  if (pwm_throttle<pwm_duty) pwm_duty--;
      
  if (lem_amps >= s) pwm_duty-=r; //pwm duty softcut
  if (lem_amps >= s) pwm_duty-=10;
  if (lem_amps >= s) pwm_duty--; 
        
  
  if (pwm_duty<0)  pwm_duty=0; // keeping pwm within 0-254 limits
  if (pwm_duty>254)  pwm_duty=254;
    
      
  if (lem_amps >= h) pwm_duty =0; // hard cut current limit kills pwm output 
    
  if (brake_state == HIGH) pwm_duty =0; // Brake switch kills pwm output
    
  analogWrite(pwm_pin, pwm_duty);
  
  
  
  Serial.print("PWM percent  "); // Print value of PWM output in percent
  Serial.print(pwm_percent);
  Serial.print(" ");
  Serial.print("Throttle  "); //Print value of the throttle 0-1023
  Serial.print(pwm_throttle);
  Serial.print(" ");
  Serial.print("PWM Duty "); // Print PWM value 0 - 255
  Serial.print(pwm_duty); 
  Serial.print(" ");
  Serial.print("Current AMPS  "); // Print valve of AMPS from lem current sensor
  Serial.println(lem_amps);
  
}
