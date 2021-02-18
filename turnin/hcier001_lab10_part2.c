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

//---shared variables----
unsigned char doorLock = 0x00;
unsigned char button = 0x00;
//---

//enumeration of states
enum doorlock_States{getKey};

int doorlock_Tick(int state){

	switch(state){ //transitions
		case getKey:
			state = getKey;
			break;
		default:
			state = getKey;
			break;
		}
	switch(state) { //actions
		case getKey:
			button = GetKeypadKey();
			break;
		default:
			break;
	}
	return state;
}

enum LockOrUnlock_States{lock, starting, one_key, two_key, three_key, four_key, five_key, unlock};

int LockOrUnlock_Tick(int state){
	switch (state){
		case lock:
			if(button == '#'){
				state = starting;
			}
			break;
		case starting:
			if(button == '1'){
				state = one_key;
			}
			else if(button == '\0'){
				state = starting;
			}
			else {
				state = lock;
			}
			break;
		case one_key:
			if(button == '2'){
				state = two_key;
			}
			else if(button == '#'){
				state = starting;
			}
			else if(button == '\0'){
				state = one_key;
			}
			else{
				state = lock;
			}
			break;
		case two_key:
			if (button == '3'){
				state = three_key;
			}
			else if(button == '#'){
				state = starting;
			}
			else if(button == '\0'){
				state = two_key;
			}
			else {
				state = lock;
			}
			break;
		case three_key:
			if(button == '4'){
				state = four_key;
			}
			else if(button == '#'){
				state = starting;
			}
			else if(button == '\0'){
				state = three_key;
			}
			else{
				state = lock;
			}
			break;
		case four_key:
			if(button == '5'){
				state = five_key;
			}
			else if(button == '#'){
				state = starting;
			}
			else if(button == '\0'){
				state = four_key;
			}
			else{
				state = lock;
			}
			break;
		case five_key:
			 if(button == '#'){
				 state = starting;
			 }
			 else if(button == '\0'){
				 state = unlock;
			 }
			 else{
				 state = lock;
			 }
			break;
		case unlock:
			if(button != '\0'){
				state = lock;
			}
			break;
		default:
			state = lock;
			break;
		}
		
	switch(state){
		case lock:
			PORTB = 0x00;
			break;
		case starting:
			PORTB = 0x80;
			break;
		case one_key:
			PORTB = 0xc0;
			break;
		case two_key:
			PORTB = 0xe0;
			break;
		case three_key:
			PORTB = 0xf0;
			break;
		case four_key:
			PORTB = 0xf8;
			break;
		case five_key:
			PORTB = 0xfc;
			break;
		case unlock:
			PORTB = 0xfe;
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
	task1.TickFct = &doorlock_Tick;

	task2.state = start;
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &LockOrUnlock_Tick;

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
