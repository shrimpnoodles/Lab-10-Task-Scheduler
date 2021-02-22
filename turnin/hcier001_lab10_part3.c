/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: https://drive.google.com/file/d/1FJnjcwgZv0YqDMzgZpz0wzEP9o-SCQje/view?usp=sharing
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


void set_PWM(double frequency){
	static double current_frequency;
	
	if(frequency!=current_frequency){
		if(!frequency){TCCR3B&=0X08;}
		else{ TCCR3B |= 0x03;}

		if(frequency<0.954){ OCR3A = 0xFFFF;}
	
		else if(frequency > 31250) {OCR3A = 0x0000;}

		else{ OCR3A = (short) (8000000 / (128 * frequency)) -1;}

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1<< COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


//---shared variables----
unsigned char doorLock = 0x00;
unsigned char button = 0x00;
unsigned char lockButton;
unsigned char doorbell_Button;
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
			if(lockButton == 0x80 && doorLock == 0X01){
				state = lock;
			}
			break;
		default:
			state = lock;
			break;
		}
		
	switch(state){
		case lock:
			doorLock = 0x00;
			PORTB = 0x00;
			break;
		case starting:
			doorLock = 0x00;
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
			doorLock = 0x01;
			PORTB = 0xfe;
		default:
			break;
		}
	return state;
}

enum doorbell_States{ off, ring, wait, wait2, wait3};
unsigned char counter = 15;
int doorbell_Tick(int state){
	switch(state){ 
		case off:
			if(doorbell_Button == 0x80 && doorLock == 0X00){
				state = ring;
			}
			break;
		case ring:
			if(counter != 0){
				state = wait;
			}
			else{
				state = off;
			}
			break;
		case wait:
			if(counter !=0){
				state = wait2;
			}
			else {
				state = off;
			}
		case wait2:
			if(counter != 0){
				state = wait3;
			}
			else {
				state = off;
			}
			break;
		case wait3:
			if(counter!=0){
				state = ring;
			}
			else{
				state = off;
			}
		default:
			state = off;
			break;
		}
	switch(state) { //actions
		case off:
			set_PWM(0);
			counter = 15;
			break;
		case ring:
			set_PWM(2936);
			counter--;
			break;
		case wait:
			set_PWM(3920);
			counter--;
			break;
		case wait2:
			set_PWM(4400);
			counter--;
			break;
		case wait3:
			set_PWM(2300);
			counter--;
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
	
	lockButton = 0x00;
	doorbell_Button = 0x00;

	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
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

	task3.state = start;
	task3.period = 200;
	task3.elapsedTime = task2.period;
	task3.TickFct = &doorbell_Tick;

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
	
	PWM_on();

    /* Insert your solution below */
    while (1) {
	lockButton = ~PINA;
	doorbell_Button = ~PINA;

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
