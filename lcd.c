/************************************************************************ *********
 * Trascau Silvia 442D
 * VeloComputer
 * Implementare modul LCD
 *********************************************************************************/


#include "lcd.h" 


/*****************************
 * IMPLEMENTARE API LCD.     *
 *****************************/

void LCD_reset()
{
	LcdDATA_DDR |=  (1<<LcdD4)|(1<<LcdD5)|(1<<LcdD6)|(1<<LcdD7);	// setam pinii de date ca pini de iesire
	LcdCMD_DDR  |=  (1<<LcdRS)|(1<<LcdRW)|(1<<LcdE);				// setam pinii de comenzi ca pini de iesire
	
	LcdCMD_PORT |= _BV(LcdRS)|_BV(LcdRW)|_BV(LcdE)|_BV(LcdD7)|_BV(LcdD6)|_BV(LcdD5)|_BV(LcdD4);

	_delay_ms(20);

	LcdCMD_PORT   &= ~(1<<LcdRS);									// Setam linia RS pe low
	LcdCMD_PORT   &= ~(1<<LcdRW);									// Setam linia RW pe low (acum suntem in modul de trimis instructiuni)
	LcdDATA_PORT  &= ~(1<<LcdD6)&~(1<<LcdD7); 		// Specificam ca vrem 4 fire de date, prima comanda (LcdD5 activ, restul nu)
	LcdDATA_PORT  |= (1<<LcdD4)|(1<<LcdD5);
	LcdCMD_PORT |=  (1<<LcdE);					// Setam linia E(nable) pe high; aceasta ii specifica LCD-ului sa preia datele
	_delay_us(600);					// Asteptam o perioada de timp T
	LcdCMD_PORT &= ~(1<<LcdE );				// Setam linia E(nable) pe low; transferul s-a terminat

	_delay_ms(10);

	LcdCMD_PORT   &= ~(1<<LcdRS);									// Setam linia RS pe low
	LcdCMD_PORT   &= ~(1<<LcdRW);									// Setam linia RW pe low (acum suntem in modul de trimis instructiuni)
	LcdDATA_PORT  &= ~(1<<LcdD6)&~(1<<LcdD7); 		// Specificam ca vrem 4 fire de date, prima comanda (LcdD5 activ, restul nu)
	LcdDATA_PORT  |= (1<<LcdD4)|(1<<LcdD5);
	LcdCMD_PORT |=  (1<<LcdE);					// Setam linia E(nable) pe high; aceasta ii specifica LCD-ului sa preia datele
	_delay_us(600);					// Asteptam o perioada de timp T
	LcdCMD_PORT &= ~(1<<LcdE );				// Setam linia E(nable) pe low; transferul s-a term

	_delay_ms(1);

	LcdCMD_PORT   &= ~(1<<LcdRS);									// Setam linia RS pe low
	LcdCMD_PORT   &= ~(1<<LcdRW);									// Setam linia RW pe low (acum suntem in modul de trimis instructiuni)
	LcdDATA_PORT  &= ~(1<<LcdD6)&~(1<<LcdD7); 		// Specificam ca vrem 4 fire de date, prima comanda (LcdD5 activ, restul nu)
	LcdDATA_PORT  |= (1<<LcdD4)|(1<<LcdD5);
	LcdCMD_PORT |=  (1<<LcdE);					// Setam linia E(nable) pe high; aceasta ii specifica LCD-ului sa preia datele
	_delay_us(600);					// Asteptam o perioada de timp T
	LcdCMD_PORT &= ~(1<<LcdE );				// Setam linia E(nable) pe low; transferul s-a term

	_delay_ms(1);

	LcdCMD_PORT   &= ~(1<<LcdRS);									// Setam linia RS pe low
	LcdCMD_PORT   &= ~(1<<LcdRW);									// Setam linia RW pe low (acum suntem in modul de trimis instructiuni)
	LcdDATA_PORT  &= ~(1<<LcdD4)&~(1<<LcdD6)&~(1<<LcdD7); 		// Specificam ca vrem 4 fire de date, prima comanda (LcdD5 activ, restul nu)
	LcdDATA_PORT  |=  (1<<LcdD5);									// setam pinii de comenzi ca pini de iesire
	LcdCMD_PORT |=  (1<<LcdE);					// Setam linia E(nable) pe high; aceasta ii specifica LCD-ului sa preia datele
	_delay_us(600);					// Asteptam o perioada de timp T
	LcdCMD_PORT &= ~(1<<LcdE );				// Setam linia E(nable) pe low; transferul s-a terminat

	_delay_ms(1);

}

void LCD_init()
{
	LCD_reset();

	LcdDATA_DDR |=  (1<<LcdD4)|(1<<LcdD5)|(1<<LcdD6)|(1<<LcdD7);	// setam pinii de date ca pini de iesire
	LcdCMD_DDR  |=  (1<<LcdRS)|(1<<LcdRW)|(1<<LcdE);				// setam pinii de comenzi ca pini de iesire

	LCD_writeInstruction(0x28); 		// Incarcam comanda: 4 bit data, 2 lines, 5x8 font
	LCD_writeInstruction(0x0C); 		// Display On, Cursor On, Blinking On
	LCD_writeInstruction(0x01);			// Clear Display

	LCD_writeInstruction(0x06);					// Increment, no shift
	
	LCD_writeInstruction(LCD_INSTR_returnHome);

}

void LCD_writeInstruction(unsigned char _instruction)
{
	/* TODO */					
	// asteptam ca LCD-ul sa fie liber sa primeasca comenzile	
	LCD_waitNotBusy();	
	
	// setam pinul RS pe low (low=instructiuni, high=date)
	RS_LOW();
	
	// setam pinul RW pe low (suntem in modul de comenzi acum)
	RW_LOW();
	
	// apelam procedura ce trimite byte-ul pe firele de date
	LCD_write(_instruction);
}


void LCD_writeData(unsigned char _data)
{
        /* TODO */
	// similar cu LCD_writeInstruction
		
	// asteptam ca LCD-ul sa fie liber sa primeasca comenzile	
	LCD_waitNotBusy();	
	
	// setam pinul RS pe low (low=instructiuni, high=date)
	RS_HIGH();
	
	// setam pinul RW pe low (suntem in modul de comenzi acum)
	RW_LOW();
	
	// apelam procedura ce trimite byte-ul pe firele de date
	LCD_write(_data);
}

void LCD_write(unsigned char _byte)
{
	/* TODO */
	// scriem cei mai semnificativi 4 biti pe firele de date
	unsigned char aux = _byte & 0xf0;
	aux >>= 1;
	LcdDATA_DDR = 0xff;
	LcdDATA_PORT &= 0b10000111;
	LcdDATA_PORT |= aux;
	
	LcdCMD_DDR = 0xff;
	// Setam Pinul E pe high
	ENABLE();
	
	// Asteptam o perioada de timp T
	_delay_us(500);
	
	// Setam Pinul E pe low
	DISABLE();
	
	// Asteptam o perioada de timp T
	_delay_us(500);
	
	// scriem cei mai putin semnificativi 4 biti pe firele de date
	aux = _byte & 0x0f;
	aux <<= 3;
	LcdDATA_DDR = 0xff;
	LcdDATA_PORT &= 0b10000111;
	LcdDATA_PORT |= aux;
	
	// Setam Pinul E pe high
	ENABLE();
	
	// Asteptam o perioada de timp T
	_delay_us(500);
	
	// Setam Pinul E pe low
	DISABLE();
}

void LCD_waitNotBusy()
{
	volatile unsigned char i,j;
    for(i=0;i<10;i++)        //A simple for loop for delay
		for(j=0;j<255;j++);
}

void LCD_waitInstructions(unsigned char _instructions)
{
	volatile int instr = _instructions;
	while (instr--)
		;
}

void LCD_printFloat2r(double _n){
	char number[16], aux;
	int i = 2;
	int n = (int)_n;
	
	number[0] = (int)((_n - n) * 10) + '0';
	number[1] = '.';
	
	if (n == 0)
		number[i++] = '0';
	else
		while (n > 0){
			number[i++] = (n % 10) + '0';
			n /= 10;
		}
	
	number[i] = '\0';
	
	for (n = 0, i--; n < i; n++, i--){
		aux = number[n];
		number[n] = number[i];
		number[i] = aux;
	}
	
	LCD_print(number);
}
		

void LCD_printDecimal2u(unsigned int _n)
{
	unsigned char tmp=0;
	
	// Extragem sutele
	while(_n>=100)
		_n-=100;

	while(_n>=10){
		tmp++;
		_n-=10;
	}

	LCD_writeData(tmp+'0');
	LCD_writeData(_n+'0');
}

void LCD_printHexa(unsigned int _n)
{
	unsigned char _tmp = _n>>4;
	if (_tmp>9)
		_tmp += 'A'-10;
	else
		_tmp += '0';
	LCD_writeData( _tmp );
	_tmp = _n & 0x0F;
	if (_tmp>9)
		_tmp += 'A'-10;
	else
		_tmp += '0';
	LCD_writeData( _tmp );
}


void LCD_print(char* _msg)
{
	unsigned char i=0;
	for( ; _msg[i]!=0 && i<16; i++)
		LCD_writeData( _msg[i] );
}

void LCD_print2(char* _msg1, char* _msg2)
{
	LCD_writeInstruction(LCD_INSTR_clearDisplay);
	LCD_print(_msg1);
	LCD_writeInstruction(LCD_INSTR_nextLine);
	LCD_print(_msg2);
}
