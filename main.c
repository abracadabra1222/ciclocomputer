/************************************************************************ *********
 * Trascau Silvia
 * VeloComputer
 * Implementare modul principal
 *********************************************************************************/

#include <avr\interrupt.h>
#include "lcd.h"

/************************************************************************ 
 * DEFINE-uri pentru parametrizarea modulului LCD
 ************************************************************************/

#define Button_DDR		DDRD				//Butoanele de comanda sunt pe portul D
#define Button_PORT		PORTD
#define Button_PIN		PIND
#define Button1			PD1					//Butonul 1 (stanga)
#define Button2			PD0					//Butonul 2 (dreapta)
#define Button3			PD6					//Butonul 3 (de pe placuta)

#define Sensor_DDR		DDRB
#define Sensor_PORT		PORTB
#define Sensor_PIN		PINB
#define Sensor			PB0					//Sensorul Hall


#define SPEED 		"Speed"
#define DISTANCE 	"Distance"
#define AVG_SPEED 	"Average speed"
#define TOTAL_TIME 	"Total Time"
#define MAX_SPEED 	"Max Speed"
#define IDLE		"VeloComputer"
#define AUTHOR		"Trascau Silvia"
#define RESET		"Reset"
#define CONFMODE	"Configure"

#define WS_CONF		"Wheel size"

#define PI				3.1416

#define TRUE			1
#define FALSE			0

#define NR_FUNCTIONS	5

#define STATE_SPD		0
#define	STATE_DIST		1
#define STATE_AVG		2
#define	STATE_MAX		3
#define STATE_TIME		4
#define STATE_IDLE		5
#define STATE_RESET		6
#define STATE_CONF		7


char state = 0;
double diameter =1.32;

double speed;
double distance = 0;
unsigned int time = 0, time_hundred = 0;
double avg_speed;
double max_speed;

char mesaj[16];
int x100 = 0, idle;
char stopped, confmode;
char reset, conf;

int w_size_inch[7] = {16, 18, 20, 22, 24, 26, 29};
float w_size_m[7] = {0.812, 0.914, 1.016, 1.116, 1.218, 1.320, 1.472};


//Functia de resetare a tuturor valorilor
void resetValues(){
	speed = 0.0;
	distance = 0.0;
	avg_speed = 0.0;
	max_speed = 0.0;
	time = 0;
	time_hundred = 0;
}


//Incrementeaza starea - ce informatii se vor afisa
void incrementState(){
	state++;
	if (state == NR_FUNCTIONS)
		state = 0;
}


//Functia de afisare a unei stari
void printState(_state){
	LCD_writeInstruction(LCD_INSTR_clearDisplay);
	LCD_writeInstruction(LCD_INSTR_returnHome);
	
	switch(_state){
		//Afiseaza viteza momentana
		case STATE_SPD:
			LCD_print(SPEED);
			LCD_writeInstruction(LCD_INSTR_nextLine);
			LCD_printFloat2r(speed);
			break;
		//Afiseaza distanta totala
		case STATE_DIST:
			LCD_print(DISTANCE);
			LCD_writeInstruction(LCD_INSTR_nextLine);
			sprintf(mesaj, "%.2d.%.2d KM", (int)(distance / 1000), (int)(((int)distance % 1000) / 10));
			LCD_print(mesaj);
			break;
		//Afiseaza viteza medie
		case STATE_AVG:
			LCD_print(AVG_SPEED);
			LCD_writeInstruction(LCD_INSTR_nextLine);
			LCD_printFloat2r(avg_speed);
			break;
		//Afiseaza viteza maxima
		case STATE_MAX:
			LCD_print(MAX_SPEED);
			LCD_writeInstruction(LCD_INSTR_nextLine);
			LCD_printFloat2r(max_speed);
			break;
		//Afiseaza timpul total
		case STATE_TIME:
			LCD_print(TOTAL_TIME);
			LCD_writeInstruction(LCD_INSTR_nextLine);
			sprintf(mesaj, "%.2d:%.2d:%.2d", time / 3600, (time / 60) % 60, time % 60);
			LCD_print(mesaj);
			break;
		//Afiseaza mesajul din starea idle
		case STATE_IDLE:
			LCD_print2(IDLE, AUTHOR);
			break;
		//Afiseaza mesajul de reset
		case STATE_RESET:
			LCD_print(RESET);
			break;
		//Afiseaza mesajul de configurare
		case STATE_CONF:
			LCD_print(CONFMODE);
			break;
		default:
			break;
	}
}

void init_timer(){
	//Initializez timer1 pentru a masura timpul
	TCCR1A = 0x00;
	//Setez prescaler = 1024 si CTC
	TCCR1B = _BV(WGM12) | _BV(CS10) | _BV(CS12);
	//Configurez timer-ul la 100 de intreruperi pe secunda
	OCR1A = 156;
	//Activez intreruperea
	TIMSK |= _BV(OCIE1A);
	x100 = 300;
}

//Intreruperea pentru timer - actualizez numarul de sutimi
ISR(TIMER1_COMPA_vect)
{
	x100 = (x100 <= 300) ? (x100 + 1) : x100;
	idle = (idle <= 2000) ? (idle + 1) : idle;
	
	if (idle == 2000){
		//Nu a fost detectata nicio miscare sau apasare de buton
		//in ultimele 20 de secunde - se trece in starea idle
		printState(STATE_IDLE);
	}
	if (x100 == 300){
		//Nu a fost sesizata vreo miscare in ultimele 3 secunde
		//Se trece in starea STOPPED
		speed = 0;
		stopped = TRUE;
		printState(state);
	}
}

//Folosesc timer-ul 0 pentru intreruperea data de senzor
void init_sensor(){
	Sensor_DDR &= ~_BV(Sensor);
	Sensor_PORT |= _BV(Sensor);
	idle = 0;
	stopped = TRUE;
	
	TCCR0 = _BV(WGM01) | _BV(CS02) | _BV(CS01);
	OCR0 = 1;
	TIMSK |= _BV(OCIE0);
}
//Senzorul detecteaza o rotatie
ISR(TIMER0_COMP_vect){
	if (!stopped){
		//Se actualizeaza timpul total
		time_hundred += x100;
		time += (time_hundred / 100);
		time_hundred = (time_hundred % 100);
		//Se actualizeaza viteza momentana
		speed = (double)(PI * diameter * 3600) / (x100 * 10);
		//Se actualizeaza viteza maxima
		if (speed > max_speed)
			max_speed = speed;
		//Se actualizeaza distanta
		distance += PI * diameter;
		//Se actualizeaza viteza medie
		avg_speed = (distance * 3600) / (time * 1000); 
		
		printState(state);
	}
	else{
		idle = 0;
		stopped = FALSE;
		printState(state);
	}
	
	x100 = 0;
	idle = 0;
}

int main()
{
	int i = 0;
	//Initializez LCD-ul
	LCD_init();
	//Activez butoanele
	Button_DDR &= ~_BV(Button1) & ~_BV(Button2) & ~_BV(Button3);
	Button_PORT |= _BV(Button1) | _BV(Button2) | _BV(Button3);
	//Activez senzorul
	Sensor_DDR &= ~_BV(Sensor);
	Sensor_PORT |= _BV(Sensor);
	
	TIMSK = 0;
	init_timer();
	init_sensor();
	printState(STATE_IDLE);
	//Activez intreruperile
	sei();
	
	while(1){
		Button_PORT |= _BV(Button1) | _BV(Button2) | _BV(Button3);
		_delay_ms(1);
		//Daca e in modul de configurare
		if (confmode){
			//Button 1 schimba dimensiunea rotii
			if ((Button_PIN & (1 << Button1)) == 0){
				i = (i + 1) % 7;
				sprintf(mesaj, "%d''", w_size_inch[i]);
				LCD_print2(WS_CONF, mesaj);
				_delay_ms(300);
			}
			//Button 2 seteaza dimensiunea rotii
			if ((Button_PIN & (1 << Button2)) == 0){
				diameter = w_size_m[i];
				confmode = FALSE;
				i = 0;
				
				LCD_writeInstruction(LCD_INSTR_clearDisplay);
				LCD_writeInstruction(LCD_INSTR_returnHome);
				LCD_print("Wheel size set.");
				_delay_ms(500);
				
				resetValues();
				printState(state);
				_delay_ms(500);
				sei();
			}
			continue;
		}
		//Nu sunt in modul de configure
		//Butonul 1 schimba informatiile afisate
		if ((Button_PIN & (1 << Button1)) == 0){
			cli();
			incrementState();
			printState(state);
			idle = 0;
			sei();
			_delay_ms(200);
		}
		//Butonul 2 incearca resetarea valorilor - trebuie apasat timp de 3 secunde
		if ((Button_PIN & (1 << Button2)) == 0){
			if (reset){
				i++;
			}
			else{
				i = 0;
				cli();
				printState(STATE_RESET);
			}
			
			idle = 0;
			reset = 1;
			
			if (i % 2)
				LCD_writeInstruction(LCD_INSTR_clearDisplay);
			else
				printState(STATE_RESET);
			_delay_ms(500);

			if (i == 5){
				resetValues();
				printState(state);
				_delay_ms(1000);
				reset = 0;
				sei();
			}
		}
		else{
			if (reset){
				printState();
				sei();
			}
			reset = 0;
		}
		//Butonul 3 incearca setarea dimensiunilor rotii
		//Se poate seta dimensiunea rotii doar daca nu se ruleaza
		//Trebuie mentinut apasat butonul timp de 3 secunde
		if (stopped && (Button_PIN & (1 << Button3)) == 0){
			if (conf){
				i++;
			}
			else{
				i = 0;
				cli();
				printState(STATE_CONF);
			}
			
			idle = 0;
			conf = 1;
			
			if (i % 2)
				LCD_writeInstruction(LCD_INSTR_clearDisplay);
			else
				printState(STATE_CONF);
			_delay_ms(500);
			
			if (i == 5){
				i = 0;
				sprintf(mesaj, "%d''", w_size_inch[i]);
				LCD_print2(WS_CONF, mesaj);
				confmode = TRUE;
				conf = 0;
				_delay_ms(1000);
			}
		}
		else{
			if (conf){
				printState();
				sei();
			}
			conf = 0;
		}
		
	}

	return 0;
}


