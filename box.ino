// door code 
#include <Keypad.h>
#include<EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char password[4];
char initial_password[4],new_password[4];
int vcc=11;
int i=0;
int relay_pin = 5;
char key_pressed=0;
const byte rows = 4; 
const byte columns = 3; 
char hexaKeys[rows][columns] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte row_pins[rows]={10,11,12,13};
byte column_pins[columns]={7,8,9};
Keypad keypad_key=Keypad( makeKeymap(hexaKeys),row_pins,column_pins,rows,columns);

void setup(){
  Serial.begin(9600);
  Serial.begin(9600);
lcd.begin();
lcd.backlight();
pinMode(relay_pin, OUTPUT);
pinMode(vcc, OUTPUT);

lcd.clear();
lcd.print(" WELCOME!! ");
lcd.setCursor(0,1);
lcd.print(" ENJOY DINING!!");
delay(3000);
lcd.clear();

lcd.print("Enter Password");
lcd.setCursor(0,1);
initialpassword();
}


void loop(){

  digitalWrite(relay_pin,HIGH);

  // admin can change the password

  key_pressed = keypad_key.getKey();
  if(key_pressed=='#')
      change();
  if (key_pressed){
    password[i++]=key_pressed;
    lcd.print(key_pressed);
    }

  if(i==4){
    delay(200);
    for(int j=0;j<4;j++)
      initial_password[j]=EEPROM.read(j);
      if(!(strncmp(password, initial_password,4))){
          lcd.clear();
          lcd.print("Pass Accepted");
          digitalWrite(relay_pin,LOW);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("READY TO GO");
          while(true){
            if(Serial.available() > 0 ){
              String receivedMessage = Serial.readStringUntil("\n");
              Serial.println(receivedMessage);
              delay(1000);
              if(receivedMessage.startsWith("01")){
                Serial.println("op");
                break;
              }
            }
          }
          Serial.println("hi");
          if(true ){
            lcd.clear();
            lcd.print("Enter Password:");
            lcd.setCursor(0,1);
            i = 0;
            Serial.println("hiop");
            Serial.println("sending");
            key_pressed = keypad_key.getKey();
            if (key_pressed){
              password[i++]=key_pressed;
              lcd.print(key_pressed);
            }
            
            if(i==4){
              delay(200);
              for(int j=0;j<4;j++)
                initial_password[j]=EEPROM.read(j);
              if(!(strncmp(password, initial_password,4))){
                lcd.clear();
                lcd.print("Pass Accepted");
                digitalWrite(relay_pin,LOW);
                delay(2000);
                i = 0;
                Serial.println("sending");
                digitalWrite(relay_pin, HIGH);
                lcd.clear();
                lcd.print("Tasty");
                lcd.setCursor(0,1);
                lcd.print(" Bites!!");
                delay(1000);

                Serial.println("doorclosed");
                delay(1000);
                Serial.println("doorclosed");
                delay(1000);

              }
              else{
                digitalWrite(relay_pin, HIGH);
                lcd.clear();
                lcd.print("Wrong Password");
                lcd.setCursor(0,1);
                lcd.print("RETURNING!!");
                delay(1000);
                lcd.clear();
                lcd.print("Tasty");
                lcd.setCursor(0,1);
                lcd.print(" Bites!!");

                i = 0;
                Serial.println("doorclosed");
                delay(1000);
                Serial.println("doorclosed");
                delay(1000);
              }
            }   
          
        }
      }
  else{
    digitalWrite(relay_pin, HIGH);
    lcd.clear();
    lcd.print("Wrong Password");
    lcd.setCursor(0,0);
    lcd.print("Pres >#< to");
    lcd.setCursor(0,1);
    lcd.print("change the pass");
    delay(2000);
    lcd.clear();
    lcd.print("Enter Password");
    lcd.setCursor(0,1);
    i=0;
    }
  }
}




void change(){
  int j=0;
lcd.clear();
lcd.print("Current Password");
lcd.setCursor(0,1);
while(j<4){
char key=keypad_key.getKey();
if(key)
{
new_password[j++]=key;
lcd.print(key);
}
key=0;}
delay(500);

if((strncmp(new_password, initial_password, 4))){
lcd.clear();
lcd.print("Wrong Password");
lcd.setCursor(0,1);
lcd.print("Try Again");
delay(1000);}

else{
j=0;
lcd.clear();
lcd.print("New Password:");
lcd.setCursor(0,1);

while(j<4){
char key=keypad_key.getKey();
if(key)
{
initial_password[j]=key;
lcd.print(key);
EEPROM.write(j,key);
j++;}}
lcd.print("Pass Changed");
delay(1000);}
 
lcd.clear();
lcd.print("Enter Password");
lcd.setCursor(0,1);
key_pressed=0;
}

void initialpassword(){

int j;
for(j=0;j<4;j++)
EEPROM.write(j,j+49);
for(j=0;j<4;j++)
initial_password[j]=EEPROM.read(j);

}