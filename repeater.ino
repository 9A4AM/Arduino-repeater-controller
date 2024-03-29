//Repeater controler by ****** 9 A 4 A M *****  Mario@2017

//Main piece of code is from Author: Adrian Florescu YO3HJV
//                                   http://www.yo3hjv@blogspot.com


 /////////////// Simple Arduino CW Beacon //////////////////

 // Written by Mark VandeWettering K6HX
 // Email: k...@arrl.net
 //
 // Thx Mark de YO3HJV !
 //
 struct t_mtab { char c, pat; } ;
 struct t_mtab morsetab[] = {
       {'.', 106},
     {',', 115},
     {'?', 76},
     {'/', 41},
     {'A', 6},
     {'B', 17},
     {'C', 21},
     {'D', 9},
     {'E', 2},
     {'F', 20},
     {'G', 11},
     {'H', 16},
     {'I', 4},
     {'J', 30},
     {'K', 13},
     {'L', 18},
     {'M', 7},
     {'N', 5},
     {'O', 15},
     {'P', 22},
     {'Q', 27},
     {'R', 10},
     {'S', 8},
     {'T', 3},
     {'U', 12},
     {'V', 24},
     {'W', 14},
     {'X', 25},
     {'Y', 29},
     {'Z', 19},
     {'1', 62},
     {'2', 60},
     {'3', 56},
     {'4', 48},
     {'5', 32},
     {'6', 33},
     {'7', 35},
     {'8', 39},
     {'9', 47},
     {'0', 63}
         } ;
 #define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))
 #define CW_SPEED  (18)
 #define DOTLEN  (1200/CW_SPEED)
 #define DASHLEN  (3.5*(1200/CW_SPEED))  // CW weight  3.5 / 1

 ////////// end CW section ////////////

 ///////// define repeater beacon Messages ////////////

     #define  RPT_call       ("9A0VMG")   // Call sign of the repeater 
     
    // #define  RPT_ctcss      ("79.7")    // CTCSS tone uncomment if needed

     #define  RPT_loc        ("JN85JO")    // Repeater QTH locator    
     #define  END_msg        ("I")        // Goodby Message
     #define  TOT_msg        ("5")        // Total Message
     


 //CW   tone output
     #define  CW_pin          (5)     // Pin for CW audio out
     #define  CW_PITCH        (900)   // CW pitch 
     
     
     
 // PANEL LEDs

    #define ledtx            (2)    // pin for TX LED
    #define ledbusy            (3)    // pin for Busy LED
    #define ledrepeat            (11)    // pin for Repeat ON LED
   // #define RPT               (10)   

                           int rx = 6;                        // Carrier Detect digital input pin
                           int ptt = 12;                      // PTT digital output pin
                           
                               float slaV = 0;                //  variable to store voltage from the ADC after the resistive divider
                               int vMath = 0;                 //  variable used for maths with ADC and voltage
                               float volts = 12;               //  volts from ADC readings
    
                    unsigned long previousMillis = 0;         // will store last time beacon transmitted            
                    const long interval = 1800000;            // 60 min interval at which to beacon (milliseconds)  3600000 15 min=900000  
                    
                    unsigned long previousSec = 0;         // will store last time beacon transmitted            
                    const long Sec = 1000;            // 60 min interval at which to beacon (milliseconds)  3600000 15 min=900000  10000=10sec
                    //  boolean TOT = false;
                               long Total = 0;
                             //  long TimeOut = 30;    // TimeOut in seconds  30 = 30 sec
               
    
                               int cDet = 0;                  // current state of the Carrier Detect
                               int lastcDet = 0;              // previous state of the Carrier Detect
     
    
   
         void setup () {
      
             Serial.begin(9600);              // for debugging
                
                pinMode(CW_pin, OUTPUT);      // Output pin for beacon and audio
                 //  digitalWrite(CW_pin, LOW);           // CW pin init
     
                pinMode(10, INPUT) ;          //  RPT ON/OFF      ####Before#### Carrier Detect on pin 10 with transistor, tied up with 10 kOhm. 
                                              //  when signal is received goes LOW. 
                pinMode(9, INPUT) ;          // Beep on/off 
                pinMode(8, INPUT);           // Enable disable total timer (timeout)
                pinMode(7, INPUT) ;             // Beacon Enable disable                 
                pinMode(12, OUTPUT);          //  Here we have PTT. When transmission is needed, this pin goes HIGH.
                
                pinMode(2, OUTPUT);           //  TX LED
                pinMode(3, OUTPUT);           //  Busy LED
                pinMode(11, OUTPUT);           //  Repeat LED
                digitalWrite(ledrepeat, HIGH);    // Always ON for now 
              // Start SEND ID first time - on startup
                                        if (digitalRead(7) == LOW) {  
                                             digitalWrite(ptt, HIGH);                 // Start Tx
                                             digitalWrite(ledtx, HIGH);               // PTT LED ON
                                             delay(100);                              // Wait a little
                                              beacon1();                              // Transmitting Audio Beacon 1 
                                             delay(100);                              // Wait a little
                                             digitalWrite(ptt, LOW);                  // Go in StandBy
                                             digitalWrite(ledtx, LOW);                // LED TX OFF
              }
              
                
                }
   
     /* ******************************** Functions here  ************************ */
     
         // What it does when signal is receiving.
         
         void repeat ()    
                    {
                      
                      
                 cDet = digitalRead(rx);
                
                 
                               if (cDet != lastcDet) {                 // if the state has changed
                               // digitalWrite(ledbusy, !digitalRead(ledbusy));         //  Busy LED ON
                                    //unsigned long currentMillis = millis();     // Read millis (from first start of the board) and load value to current Millis variable                                
                                    if (cDet == LOW) {                 // 
                                    
                                                                       // went from off to on:
                                    digitalWrite(ptt, HIGH);           //  Tx PTT @ +5V 
                                    digitalWrite(ledtx, HIGH);         //  TX LED ON
                                    digitalWrite(ledbusy, HIGH);         //  Busy LED ON
                                 
                                   
                                                      } 
                               else {                                  // same state @cDet input
                                       digitalWrite(ledbusy, LOW);      //  Busy LED OFF
                                       delay(1200);                      // wait a little before courtesy tone
                                       // courtesy();                     // execute 'courtesy' function
                                       // delay(70);                      // Wait a little while TX
                                       if (digitalRead(rx) == LOW) {
                                       digitalWrite(ledbusy, HIGH);         //  Busy LED ON
                                         }
                                         else {
                                         //tone(5, 1200, 50);               // Beep on end of QSO
                                       beep();
                                       digitalWrite(ptt, LOW);         // put Tx PTT @ GND, go to StandBy
                                       digitalWrite(ledtx, LOW);
                                      // previousTotal = 0;
                                     //  TOT = false;
                                       }
                                    }
                              delay(50);                               // Delay a little bit to avoid bouncing
                                
                                                       }
                                                    
                  lastcDet = cDet;                                     // save the current state as the last state 
                                                                       // for next time through the loop
                    }  // END of 'repeat' function
       
       
       
       
           // Function to return the battery voltage      
           // Better to call it when transmitting to have a 'worst case' measurement       
                     
           float battery() {
                      delay(50);                                       //  hold on before ADC reading
                      int vMath = analogRead(A0);                      //  Load the SLA BAttery voltage through resistive divider on ADC A0 
                      float slaV = vMath * (15 / 1023.0);              //  slaV will be calculated in Volts  with max value 15V                        
                      return slaV;                                     //  The 'battery' function returning voltage measuring
                      }    
   
    
       // *************  MAIN LOOP  ***************
                
       void loop() { 
       // if (RPT == HIGH){
                  // digitalWrite(ledrepeat, HIGH);    //   
                
                if (digitalRead(10) == LOW){ 
                 digitalWrite(ledrepeat, HIGH);
                 
                  repeat();                 // called  at the beggining
                 
                 
                        
                 delay(100); 
                 
                 
                 
                 // TIMEOUT
                  if (digitalRead(ptt) == HIGH && digitalRead(8) == LOW) {
                    
                    
                   if (millis() - previousSec >= Sec) {
                      previousSec = millis();
                                     
                                        digitalWrite(13, HIGH);
                                     Total = Total + 1;
                      }
                    else {
                     digitalWrite(13, LOW);
                      }
                    }
                   if (digitalRead(ptt) == LOW) {
                     Total = 0;
                     }
                      
                   
                   if (Total >= 300) {                             // TIMEOUT     in seconds (5min)

                                       beacon2();
                                       digitalWrite(ptt, LOW);         // put Tx PTT @ GND, go to StandBy
                                       digitalWrite(ledtx, LOW);
                                       Total = 0;
                                       digitalWrite(13, LOW);

                           }     
                                                              // AntiKerchunk delay
                                                              // Beacon timer   
                  unsigned long currentMillis = millis();     // Read millis (from first start of the board) and load value to current Millis variable
                 
                  
                  
                  

                                 if (currentMillis - previousMillis >= interval)  {  
                                     previousMillis = currentMillis;                  // Time to beacon (INTERVAL) has come
                                     cDet = digitalRead(rx);                          // Read the Carrier Detect Input pin 
                              
                                        if (cDet == HIGH && digitalRead(7) == LOW) {                            // If not QSO on the Rx frequency
                                             digitalWrite(ptt, HIGH);                 // Start Tx
                                             digitalWrite(ledtx, HIGH);               // PTT LED ON
                                             delay(100);                              // Wait a little
                                              beacon1();                              // Transmitting Audio Beacon 1 
                                             delay(100);                              // Wait a little
                                             digitalWrite(ptt, LOW);                  // Go in StandBy
                                             digitalWrite(ledtx, LOW);                // LED TX OFF
                                                          }
                            
                                         else if (cDet == LOW) {                      // If a QSO on the RX frequency
                                            //delay(100);                               // Wait a little           
                                              //beacon2();                              // Transmitting Audio Beacon 2
                                            //digitalWrite(ptt, HIGH);   }              // Keep the repeater transmitting
                                            //digitalWrite(ledtx, HIGH);
                                            // LED TX ON
                                            }             // Remove this if use original
                                                                                    }
                                 else {}                                              // If time to beacone hasn't come (INTERVAL), do nothing      
                 
                 
                 
                 }
                 else{
                   digitalWrite(ledrepeat, LOW);
                   }
                   }
                 }  
                }
            //  else{
             // digitalWrite(ledrepeat, LOW);    // 
            // }  
            // }
             
       
   
         // **** Audio Beacon 1 - for StandBy (No QSO in progress)
         void beacon1() {
        
           //delay (500);
        
               delay (400);                                     // Wait a little
               sendmsg(RPT_call) ;                  //send call
               delay(7*DOTLEN) ;
              // sendmsg("QRV") ;                     //send info
              // delay(7*DOTLEN) ;
              //  sendmsg(RPT_ctcss);                // uncomment this two lines if there is a CTCSS enabled on the repeater
              //  delay(7*DOTLEN) ;
               sendmsg(RPT_loc) ;                   //send qth locator
               delay(3*DOTLEN) ;
              // sendmsg(END_msg);
              // delay(3*DOTLEN) ;
                          return;  }
                            
         // **** Audio Beacon 2 - For repeating (QSO in progress)
         
         void beacon2() { 
     //          delay(100);
     //        tone(5, 2880, 100);
     //   delay(200);
     //        tone(5, 1740, 100);
     //   delay(200);
     //        tone(5, 1200, 100);
     //   delay(500);
     //        tone(5, 880, 200);
      if (digitalRead(9) == LOW) {
 sendmsg(TOT_msg);
 }
     
                         return; }
     
                           
void beep(){
 if (digitalRead(9) == LOW) {
 sendmsg(END_msg);
 }
return; } 


  
     
       void courtesy() {
                                                                      // We read the voltage of the battery via 'battery' external function
                      volts = 12 ;
                      
                Serial.println();
                Serial.print(volts);
                     delay(50);                                      // Wait for the portable radio who stops transmitting to get into the receiving mode to be able to hear the tone


             if ((volts >= 0) && (volts < 10.5)) {                         // Practically, this will end the normal operation leaving only the receiver section open to monitor the frequency. Beep is just for tests.
     
                 tone(5, 440, 300);
                 delay(301);
                               }
             
             else if ( volts >= 10.5 && volts < 11.5) {     // Watch out! The battery goes low, limit comms!

                 tone(5, 1880, 200); 
                 delay(201); 
                 tone(5, 440, 200);
                 delay(201);
                                                    }
             
             
             else if ( volts >=11.5 && volts < 13.6) {     // This is OK, normal operations

                 tone(5, 2900, 70);
                 delay(71);
                                                     }
             
             else if (volts >= 13.6) {                     // Over voltage for some reasons. Not normal!

                 tone (5, 1880, 200); 
                 delay(201); 
                 tone(5, 2880, 200);                   // We have to do something to discharge the battery!
                 delay(201);
                                     }

                
                        }




 /////////////////////////    CW GENERATION ROUTINES  //////////////////////////
 void dash() {
     tone(CW_pin,CW_PITCH); 
     delay(DASHLEN);
     noTone(CW_pin);     
     delay(DOTLEN) ;
     }
 ////////////////////////////
 void dit() {
   tone(CW_pin,CW_PITCH); 
   delay(DOTLEN);
   noTone(CW_pin);    
   delay(DOTLEN);
   }
 ///////////////////////////
 void send(char c) {
   int i ;
     if (c == ' ') {
     delay(7*DOTLEN) ;
     return ;
     }
    if (c == '+') {
     delay(4*DOTLEN) ; 
     dit();
     dash();
     dit();
     dash();
     dit();
     delay(4*DOTLEN) ; 
     return ;
     }    
     
   for (i=0; i<N_MORSE; i++) {
     if (morsetab[i].c == c) {
       unsigned char p = morsetab[i].pat ;
       while (p != 1) {
           if (p & 1)
             dash() ;
           else
             dit() ;
           p = p / 2 ;
           }
       delay(2*DOTLEN) ;
      // wdt_reset();        // Reset Watchdog timer
       return ;
       }
     }
   }
 ///////////////////////////
 void sendmsg(char *str) {
   while (*str)
     send(*str++) ;
   }
   

          
 /////////////////////////  END  CW GENERATION ROUTINES  //////////////////////////
