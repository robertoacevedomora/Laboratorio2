#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


//Definir los estados
#define INICIO 0
#define SUMI_b 1
#define SUMI_m 2
#define SUMI_a 3
#define LAVA_b 4
#define LAVA_m 5
#define LAVA_a 6
#define ENJU_b 7
#define ENJU_m 8
#define ENJU_a 9
#define CENT_b 10 
#define CENT_m 11 
#define CENT_a 12


//Variables auxiliares
int buttom_inicio;
int buttom_alto;
int buttom_medio;
int buttom_bajo;  //Asi, segun el estado veo si ese boton esta en 1 o en 0. De esta forma hago un if else
int nxt_state;  //Estos son los estados
int state;

void fsm();


void pinchange_setup(void)
{
    PCMSK |= (1<<PCINT0); //Habilita el PB4 para LED //Eneable PB0 como la fuente PCIN
    PCMSK1 |= (1<<PCINT9);
    GIMSK |= (1<<INT1)|(1<<INT0)|(1<<PCIE0)|(1<<PCIE1); //Eneable pin change interrupt requests, habilito INT0
    MCUCR |= (1<<ISC01)|(1<<ISC10)|(1<<ISC11); 
    sei(); //Enable global interrupts
}


 
int main (void)
{
   // DDRB |= (1<<PB3); 
   // DDRB |= (1<<PD0); 
    DDRB = 0xFE;  //B7,B6,B4,B5 y B3 B2, B1como salidas
    DDRD =0x40;
    //PORTB |= (1<<PB0)|(1<<PB2); //Activacion de la resistencia pull-up interna para conectar un push button, pone esos pines como salidas.
    PORTB = 0x01;//0101, B0 Y B2
    PORTA = 0x01;
    pinchange_setup();

//START

    buttom_inicio = 0;
    buttom_bajo = 0;
    buttom_medio = 0;
    nxt_state = INICIO;
    state = INICIO;

 
    while(1)
    {
        state = nxt_state;
        fsm();// pinchange_setup();
        //Reemplaza este codigo con la rutina base
       
    } 
}

void fsm(){
    switch (state)
    { 
    case INICIO:
        if (buttom_inicio == 1){                 //Con este primer estado pongo a funcionar los botones y la luz que les corresponde.
            PORTB |= (1 << PB5);
            _delay_ms(500);
            if (buttom_bajo == 1){
                PORTB |= (1<<PB2);
                _delay_ms(500);
                nxt_state = SUMI_b;
            } else if (buttom_medio == 1){
                PORTB |=(1<<PB1);
                _delay_ms(500);
                nxt_state = SUMI_m;
            } else if (buttom_alto == 1){
                PORTB |=(1<<PB6);
                _delay_ms(500);
                nxt_state = SUMI_a;
        } else
            nxt_state = INICIO;   
        }
        break;
    case SUMI_b:
        PORTB |=(1<<PB4);   //B4, SUMERGIR
        _delay_ms(500);
        nxt_state = LAVA_b;
        break;
    case LAVA_b:
        PORTB |=(1<<PB4); //B3, LAVAR
        _delay_ms(500);
        nxt_state = ENJU_b;
        break;  
    case ENJU_b:
        PORTB |=(1<<PB3); //B3, LAVAR
        _delay_ms(500);
        nxt_state = LAVA_b;
        break;     
    case CENT_b:
        PORTD |=(1<<PD6); //B3, LAVAR
        _delay_ms(500);
        nxt_state = LAVA_b;
        break;            

    }
}

//Interrupciones

//Boton nivel bajo 
ISR (PCINT0_vect) //Interrupt service routine activada por PCINT0 //PB0 
{
   PCMSK &= ~(1<<PCINT0); //Disable PCINT0 para el debounce , lo mantiene apagado

        //PORTB = 0x10;    //Solo se activa este
        //_delay_ms(500);
        if (state == INICIO){
            buttom_bajo = 1;
        }
        //PORTB ^= 1<<PB3;  //PB4
    
 
    PCMSK |= 1<<PCINT0; //Enable PCINT0 antes de terminar la ISR
}

//Boton nivel medio
ISR (PCINT1_vect) 
{
    PCMSK1 &= ~(1<<PCINT9); //Disable PCINT0 para el debounce

        //PORTB = 0x02; //PB3
        //_delay_ms(150); 
        
        //PORTB ^= 1<<PB4;
        if (state == INICIO){
            buttom_medio = 1;
        }
    
 
    PCMSK1 |= 1<<PCINT9; //Enable PCINT0 antes de terminar la ISR
}

//Bonton inicio
ISR (INT0_vect) //PB1 //PD2
{

        PORTB = 0x80; //Puerto B7
        _delay_ms(150); 
        if (state == INICIO){
            buttom_inicio = 1;
        }
        
        //PORTB ^= 1<<PB4;
}

//Boton nivel alto
ISR (INT1_vect) //PB1
{

        //PORTB = 0x40; //Puerto B6
        //_delay_ms(150); 
        if (state == INICIO){
            buttom_alto = 1;
        }
        
        //PORTB ^= 1<<PB4;
}