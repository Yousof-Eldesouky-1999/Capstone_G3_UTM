#include "SPI.h"
#include "Phpoc.h"

PhpocEmail email;
PhpocDateTime datetime;

const int THRESHOLD = 50;
boolean objPresent = false;
long adcValue = 0;

boolean sendGmail();

void setup() {
    Serial.begin(9600);
    while(!Serial)
    ;
    pinMode(A0, INPUT); // to read value from sensor
	pinMode(5, OUTPUT); // to control relay
    
    Phpoc.begin(PF_LOG_SPI | PF_LOG_NET | PF_LOG_APP);
    datetime.date("Y-M-d H:i:s");
    
}

void loop() {
    // read 30 time and get average value to eliminate noises
    adcValue = 0;
    
    for(int i = 0; i < 30; i++)
    {
        adcValue += analogRead(A0);
        delay(10);
    }
    
    adcValue /= 30;
    
    if(adcValue < THRESHOLD)
    {
        
        if(objPresent)
        {
            //Alert
             digitalWrite(5, HIGH);
            // send notification
            while(!sendGmail())
               ;
            
            objPresent = false;
        }
    }
    else
    {
        objPresent = true;
        digitalWrite(5, LOW);
    }
}

boolean sendGmail(){
    // setup outgoing relay server - gmail.com
    email.setOutgoingServer("smtp.gmail.com", 587);
    email.setOutgoingLogin("your_account", "your_password");
    
    // setup From/To/Subject
    email.setFrom("your_account@gmail.com", "Arduino");
    email.setTo("receiver_account@gmail.com", "Boss");
    
    email.setSubject("Alert: Your treasure is being taken away");
    
    // write email message
    email.beginMessage();
    email.println("Location: Planet Earth.");
    email.println("");
    email.print("Time: ");
    email.println(datetime.date());
    email.println("");
    email.println("Your treasure is being taken away.");
    email.endMessage();
    
    // send email
    if(email.send() > 0)
    {
        Serial.println("Email send ok");
        return true;
    }
    else
    {
        Serial.println("Email send failed");
        return false;
    }
}
