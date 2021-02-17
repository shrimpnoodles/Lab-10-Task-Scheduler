/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#endif


/*unsigned char GetSingleButton(unsigned char row, unsigned char col){
	B = (B | 0x0f) & ~(1 << col);

	return ((A & (1 << row)) == 0 ? 1 : 0);
}*/
//---shared variables----
unsigned char button = 0x00;
//---

//enumeration of states
enum pushKeypad_States{ push_key};

int pushKeypadSM_Tick(int state){
	

	switch(state){ //transitions
		case push_key:
			state = push_key;
			break;
		default:
			state = push_key;
			break;
		}
	switch(state) { //actions
		case push_key:
		/*	if(GetSingleButton(1,1)){
				button =0x80;
			}
			else if(GetSingleButton(1,2)){
				button =0x80;
			}
			else if(GetSingleButton(1,3)){
				button = 0x80;
			}
			else if(GetSingleButton(1,4)){
				button = 0x80;
			}
			else if(GetSingleButton(2,1)){
				button =0x80;
			}
			else if(GetSingleButton(2,2)){
				button = 0x80;
			}
			else if(GetSingleButton(2,3)){
				button = 0x80;
			}
			else if(GetSingleButton(2,4)){
				button = 0x80;
			}
			else if(GetSingleButton(3,1)){
				button = 0x80;
			}
			else if(GetSingleButton(3,2)){
				button = 0x80;
			}
			else if(GetSingleButton(3,3)){
				button = 0x80;
			}
			else if(GetSingleButton(3,4)){
				button = 0x80;
			}
			else if(GetSingleButton(4,1)){
				button = 0x80;
			}
			else if(GetSingleButton(4,2)){
				button = 0x80;
			}
			else if(GetSingleButton(4,3)){
				button = 0x80;
			}
			else if(GetSingleButton(4,4)){
				button = 0x80;
			}
			else{
				button = 0x00;
			}*/
			button = GetKeypadKey();
			break;
		default:
			break;
	}
	return state;
}

enum LEDon_States{ output};


int LEDon_Tick(int state){
	switch (state){
		case output:
			state = output;
			break;
		default:
			state = output;
			break;
		}
		
	switch(state){
		case output:
		//	PORTB = button;
			if(button == '1'){
				PORTB = 0x80; // set pb 7 to 1
			}
			else if(button == '4'){
				PORTB = 0x80;
			}
			else if(button == '7'){
				PORTB = 0x80;
			}
			else if(button == '*'){
				PORTB = 0x80;
			}
			else if(button == '2'){
				PORTB = 0X80;
			}
			else if(button == '8'){
				PORTB = 0x80;
			}
			else if(button == '0'){
				PORTB = 0x80;
			}
			else if(button == '3'){
				PORTB = 0x80;
			}
			else if(button == '6'){
				PORTB = 0x80;
			}
			else if(button == '9'){
				PORTB = 0x80;
			}
			else if(button == '#'){
				PORTB = 0x80;
			}
			else if(button == 'A'){
				PORTB = 0x80;
			}
			else if(button == 'B'){
				PORTB = 0x80;
			}
			else if(button == 'C'){
				PORTB = 0x80;
			}
			else if(button == 'D'){
				PORTB = 0x80;
			}
			else{
				PORTB = 0x00;
			}
			break;
		default:
			break;
		}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xff;
	DDRB = 0XFF; PORTB = 0X00;
	DDRC = 0XF0; PORTC = 0X0F;

	static task task1, task2;
	task *tasks[] = {&task1, &task2};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &pushKeypadSM_Tick;

	task2.state = start;
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &LEDon_Tick;

	unsigned long int findGCD(unsigned long int a, unsigned long int b){
		unsigned long int c;
		while(1){
			c = a%b;
			if (c==0){
				return b;
			}
			a = b;
			b = c;
		}
		return 0;
	}
	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for(i=1; i< numTasks; i++){
		GCD = findGCD(GCD, tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();


    /* Insert your solution below */
    while (1) {
	for(i=0; i<numTasks; i++){
		if(tasks[i]->elapsedTime == tasks[i]->period){
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}
	while(!TimerFlag){};
	TimerFlag = 0;
    }
    return 1;
}
