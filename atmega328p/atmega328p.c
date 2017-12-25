#include <avr/io.h>		// for the input/output registers
#include <util/delay.h> 
#include <avr/interrupt.h>



#define CPU_FREQ        16000000L       // Assume a CPU frequency of 16Mhz

void init_serial(int speed)
{
    /* Set baud rate */
    UBRR0 = CPU_FREQ/(((unsigned long int)speed)<<4)-1;

    /* Enable transmitter & receiver */
    UCSR0B = (1<<TXEN0 | 1<<RXEN0);

    /* Set 8 bits character and 1 stop bit */
    UCSR0C = (1<<UCSZ01 | 1<<UCSZ00);

    /* Set off UART baud doubler */
    UCSR0A &= ~(1 << U2X0);
}


void send_serial(unsigned char c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}


unsigned char get_serial(void) 
{
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}


void output_init(void)
{
    DDRB |= 0x20; // LED13 PB5
}


void output_set(unsigned char value)
{
    if(value==0) PORTB &= 0xdf;
    else PORTB |= 0x20;
}


void input_init(void)
{
    DDRD = 0x00;  // PD as input
    PORTD = 0xFF; // Pull-up activated on PD
}


void init_int(void)
{
    PCICR |= (1 << PCIE2);
    PCMSK2 = 0x7C;
}


ISR(PCINT2_vect)
{
    send_serial( (((~PIND) >> 2) & 0x1F) );
}



int main(void)
{
    //initialization
    init_serial(9600);
    output_init();
    input_init();
    init_int(); //to send portD state
    sei();
    
    //light uff the led
    output_set(0);
    while(1)
    {
        //waiting for data from the host
        char c=get_serial();
        output_set(c);
    }
    
    return 0;
}
