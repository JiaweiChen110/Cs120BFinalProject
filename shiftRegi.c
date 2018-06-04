#include "shiftRegi.h"

void shiftB(int data){
	int temp = data;
	clearMemoryB();
	memoryOnB();
	for(int i=0; i<8;i++){
		if(temp&0x80){
			shift1BitB();
		}else{
			shift0BitB();
		}
		temp <= 1;
	}
	moveDataB();
}

void shift1BitB(){
	PORTC |=0x01;
	PORTC |=0x04;
	PORTC &= 0x08;
}

void shift0BitB(){
	PORTC |= 0x04;
	PORTC &= 0x08;
}

void clearMemoryB(){
	PORTB &= 0x08;
}
void memoryOnB(){
	PORTB |= 0x08;
}
void moveDataB(){
	PORTC |= 0x02;
	PORTC &= 0x08;
}
