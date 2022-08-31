/*
  Bala Srinivasan
  Simple 5 digit *DC* voltmeter (0 - 1.45V) (other digits are noisy)
  Inspired by scullcom millivoltmeter 
  14 bit ADC + 5 bit oversampling 
  msp 432 

  A1-Analog input
  A14 - Ground (autozero)
  LCD = 8*1 44780 compatible 
  LiquidCrystal lcd(RS - 10, EN - 9, D4-8, D5 -7, D6- 6, D7 -5);
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins

LiquidCrystal lcd(10, 9, 8, 7, 6, 5);
int sensorPin = A1;    // select the input pin for the potentiometer
int autoZeroPin = A14;


int ledPin = 13;      // select the pin for the LED
int cal = 0;

//cal not used  - auto zero on each sample - left for future consideration
//cal is a one time op - so more BW
const int CalButton = PUSH1;
int CAL_LED_B = BLUE_LED;


const int osr_bits = 5;
const int adc_bits = 14;

int CalSetup = 0;
long adcread;
float volt;                              //voltage reading
float v_ref = 1.450;                     //Reference Voltage used
String v;                                //string to select V, mV or uV on display


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 1);

  analogReadResolution(adc_bits);
  
  analogReference(INTERNAL1V45);

  //cal not used - it autozeros
  pinMode(CalButton, INPUT_PULLUP);


  lcd.print("DC VOLTS");
  delay(1000);
  lcd.clear();
  lcd.print("0-1.45V");
  delay(1000);
  lcd.clear();
  lcd.print("BALA");
  delay(1000);
  lcd.clear();
}


void loop() {
  

  analogReadResolution(adc_bits);

  CalSetup = digitalRead(CalButton);     
  if (CalSetup == LOW)                  
  {
    Cal_Adjust();                       
  } else {                             


    lcd.setCursor(0, 1);               

    int i;
    long sum = 0; int autoZero = 0;
    int samplecount = pow(4, osr_bits);
    for (i = 0; i < samplecount; i++)  
    {
      adcread = analogRead(sensorPin);
      autoZero = analogRead(autoZeroPin);
      sum = sum + adcread - autoZero;
    }
    sum = sum - cal;
    volt = sum;

    // *** this is the critical part for higher precision ***
    // while sum = sum >> osr_bits  works it scales back to 16384 early and everything following is limited to
    // 1 in 16384 LSB ;  keeping  all accumulated analogRead values in sum and then operating on them as a float,
    // during scaling ,keeps the noise lower; Compare
    //-------------
    // sum = sum >> osr_bits ;
    // sum = sum >> osr_bits ;
    // sum = volt;
    // volt = volt * v_ref / ((pow(2, (adc_bits)));
    //      ------VS------
    // volt = sum;
    // volt = volt * v_ref / ((pow(2, (adc_bits))) * (pow(2, osr_bits) * pow(2, osr_bits))) ;
    //    volt = volt / pow(4, osr_bits);
    //    volt = volt / pow(2, adc_bits);
    //    volt =volt*v_ref;

    //-----------------

    volt = volt / samplecount;
    volt = volt / pow(2, adc_bits);
    volt = volt * v_ref;


    lcd.setCursor(0, 1);
    
    // no units  - its a 8*1 LCD ..sad face; implied Volts
    
    lcd.print(volt, 6);


  }
  delay(200);

}


//Not used anymore; AutoZero per reading from A14
long Cal_Adjust(void) {
  //  lcd.clear();
  //  lcd.setCursor(0, 0);
  //  lcd.print("SHORTNOW");
  //  digitalWrite(CAL_LED_B, HIGH);
  //  delay(4000);
  //
  //  int i;
  //  long sum = 0;
  //  for (i = 0; i < (pow(4, osr_bits)); i++)      //loop to take sample readings
  //  {
  //    adcread = analogRead(sensorPin);               //read data from LTC2400 ADC
  //    //delay(ct);
  //    sum += adcread;
  //  }
  //
  //  cal = sum;
  //  lcd.clear();
  //
  //  lcd.setCursor(0, 1);
  //
  //  lcd.print(cal);
  //  delay(1000);
  //  lcd.clear();
  //  lcd.setCursor(0, 1);
  //
  //  digitalWrite(CAL_LED_B, LOW);
  //
  //
  //
}
