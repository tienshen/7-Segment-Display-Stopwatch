/*
 * Simple_SPI.c
 *
 * Created: 10/10/2019 10:24:16 AM
 * Author : Tien & George
 */ 
#include <unistd.h>
#include <avr/io.h>
//5#define F_CPU 3333333
//#include <util/delay.h>
 // initialize functions
void outSeg(int n);
void sendbytes(uint8_t data);
void setNum(int d,int n);
void outDigit(int n);
void sendDot();

int main(void)
{
	// SPI Master mode selected, prescaler = 0x0 = DIV4 = CLK_PER/4, SPI enabled
	SPI0.CTRLA = SPI_MASTER_bm | SPI_ENABLE_bm;
	SPI0.CTRLB = SPI_BUFEN_bm;//0b10000100;
	PORTA.DIR = PIN3_bm | PIN1_bm;
	RTC.CTRLA = 0x01;			// enable RTC with div1 (default)
	RTC.CNT = 0;				//Sets count to 0
	int tenthsec = 0;			//Initializes tenth of a second
	int sec = 0;				//Initializes seconds
	int sectens = 0;			//Initializes tens of seconds
	int min = 0;				//Initializes minutes
	PORTB.DIRSET = PIN4_bm;
	int x = 0;
	
    while (1) {	
		/*
		SPI0.DATA = 0x40;
		SPI0.INTFLAGS = 0x40;
		while ((SPI0.INTFLAGS & 0x40) != 0x40){
		} */
		if(RTC.CNT > 3277){		
			tenthsec++;			
			RTC.CNT = 0;		//Reset count to 0
			/*x = (x+1) % 2;
			if(x==0){
				PORTB.OUTSET = PIN4_bm;
			} else {
				PORTB.OUTCLR = PIN4_bm;
			} */
			//setNum(4,sec);
		}
		if(tenthsec == 10){		//Increment seconds every time tenths of a second reaches 10, resets tenth of a second and rtc count
			sec++;
			tenthsec = 0;
		}
		if(sec == 10){			//Increment tens of seconds everytime seconds reaches 10, resets seconds and rtc count
			sectens++;
			sec = 0;
		}
		if(sectens == 6){		//Increment minute everytime tens of seconds reaches 6, resets tens of seconds and rtc count
			min++;
			sectens = 0;
		}
		if(min == 10){			//When minute reaches 10, resets minutes to 0 and rtc count
			min = 0;
		}
		setNum(4, tenthsec);
		setNum(3, sec);
		setNum(2, sectens);
		setNum(1, min);
		sendDot();	
    }
}
void outSeg(int n){
	switch(n){
		case 0:
			return sendbytes(0b01111110);
		case 1:
			return sendbytes(0b00001100);
		case 2:
			return sendbytes(0b10110110);
		case 3:
			return sendbytes(0b10011110);
		case 4:
			return sendbytes(0b11001100);
		case 5:
			return sendbytes(0b11011010);
		case 6:
			return sendbytes(0b11111010);
		case 7:
			return sendbytes(0b00001110);
		case 8:
			return sendbytes(0b11111110);
		case 9:
			return sendbytes(0b11001110);
	}
}
void sendbytes(uint8_t data){
	SPI0.DATA = data;
	SPI0.INTFLAGS = 0x40;
		while ((SPI0.INTFLAGS & 0x40) != 0x40){
	}
}
void setNum(int d,int n){
	//PORTA.OUTCLR = PIN1_bm;
	outDigit(d); // send byte to decide which digit of the 4 to display
	outSeg(n); // send byte to decide what number to display
	//PORTA.OUTTGL = PIN1_bm;
	int x = RTC.CNT; // make display stay in this state for 40 counts
	while(RTC.CNT < x + 40){}	
}
void outDigit(int n){
	switch(n){
		case 0:
			return sendbytes(0b00000000);
		case 1:
			return sendbytes(0b00000010);
		case 2:
			return sendbytes(0b00000100);
		case 3:
			return sendbytes(0b00001000);
		case 4:
			return sendbytes(0b00010000);
	}
}

void sendDot() { //places the "dot" dp on the 3rd digit 
	sendbytes(0b00001001);
	sendbytes(0b00000000);	
	int x = RTC.CNT; 
	while(RTC.CNT < x + 40){}		
}