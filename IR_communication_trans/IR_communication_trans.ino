#include<avr/io.h>
#include<avr/interrupt.h> 
uint32_t ON = 0b10110010010011011111001000001101;
uint32_t OFF = 0b10110010010011010110001010011101;
volatile int k = 0;
ISR(TIMER2_OVF_vect)
{
  k++;
  //Serial.println(k);
}
void del()
{
  Serial.println("del");
  k=0;
  TCCR2B = (1<<CS20) | (1<<CS22) | (1<<CS21);
  TCNT2 = 0;
  while( k<=61) {Serial.println("While"); Serial.println(k); }
  TCCR2B =0x00;
  Serial.println("del over");
}
ISR(TIMER0_COMPA_vect)
{
  PORTD^=0xFF;
}
ISR(TIMER1_COMPA_vect)
{
  PORTD = 0x00;
  TCCR0B = 0x00;
  TCCR1B = 0x00;
}
void start_high()
{
  OCR1A = 18000;
  TCCR0B = (1<<CS00);
  TCCR1B = (1<<CS11);
  TCNT0 = 0;
  TCNT1 = 0;
  PORTD = 0xFF; 

  while(TCCR1B & (1<<CS11)) ; // wait till interrupt is fired
}
void start_low()
{
  OCR1A = 9000;
  TCCR1B = (1<<CS11);
  TCNT1 = 0;

  while(TCCR1B & (1<<CS11)) ; // WAIT TILL INTERRUPT IS FIRED
}
void pulse_high()
{
  OCR1A = 9000;
  TCCR0B = (1<<CS00);
  TCCR1B = (1<<CS10);
  TCNT0 = 0;
  TCNT1 = 0;
  PORTD = 0xFF; 

  while(TCCR1B & (1<<CS10)) ; // wait till interrupt is fired
}
void pulse_low(int ticks)
{
  OCR1A = ticks;
  TCCR1B = (1<<CS10);
  TCNT1 = 0;

  while(TCCR1B & (1<<CS10) ); // WAIT TILL INTERRUPT IS FIRED
}
void send_code( uint32_t code )
{
  start_high();
  start_low();
  for( int i=0;i<32;i++)
  {
    pulse_high();
    if (code &(0x80000000) )
    {
      pulse_low(27000);
    }
    else
    {
      pulse_low(9000);
    }
  }
  start_high();
}
int main()
{
  Serial.begin(9600);
  TCCR0A = (1<<WGM01); // t0 no prescaling
  TCCR1A =(1<<WGM12); // t1 8 CTC
  TIMSK0 = (1<<OCIE0A);
  TIMSK1 = (1<<OCIE1A);
  TIMSK2 = (1<<TOIE2);
  OCR0A = 210;  // for 38 KHz
  DDRD = 0xFF;
  PORTD = 0x00;
  sei();
    while(1)
    {
      send_code(ON);
      del();
      send_code(OFF);
      del();
      Serial.println("done"); 
    }
  return 0;
}
