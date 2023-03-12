// code to control LED pin on the ESP 01S / 12-F
String state;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  while(!Serial);
  Serial.println("Input 1 to turn LED on and 2 to to turn off");
}

void loop() {

  while(Serial.available())
  {
    state = Serial.readString();
    Serial.println(state);
    if(state == "off\n")
    {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Command completed LED turned ON");
    }
    if(state == "on\n")
    {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Command completed LED turned OFF");
    }
  }
//  digitalWrite(3, HIGH); 
//  delay(1000);            // waits for a second
//  digitalWrite(3, LOW);  
//  delay(1000);            // waits for a second
}
