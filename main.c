/*
 * FinalProject.c
 *
 * Created: 5/31/2018 8:29:55 PM
 * Author : alph_
 */ 

#include <avr/io.h>
#include <bit.h>
#include <scheduler.h>
#include <timer.h>
//#include <shiftRegi.h>


void shift1BitB(int indicator){
	if(indicator){
		PORTB |=0x01;
		PORTB |=0x04;
		PORTB &= 0x08;
	}else{
		PORTB |=0x10;
		PORTB |=0x40;
		PORTB &= 0x80;
	}
}

void shift0BitB(int indicator){
	if(indicator){
		PORTB |= 0x04;
		PORTB &= 0x08;
	}else{
		PORTB |= 0x40;
		PORTB &= 0x80;
	}
}

void clearMemoryB(int indicator){
	if(indicator){
		PORTB &= 0x08;
	}else{
		PORTB &= 0x80;
	}
}
void memoryOnB(int indicator){
	if(indicator){
		PORTB |= 0x08;
	}else{
		PORTB |= 0x80;
	}
}

void moveDataB(int indicator){
	if(indicator){
		PORTB |= 0x02;
		PORTB &= 0x08;
	}else{
		PORTB |= 0x20;
		PORTB &= 0x80;
	}
}

void shiftB(int data,int indicator){
	int temp = data;
	clearMemoryB(indicator);
	memoryOnB(indicator);
	for(int i=0; i<8;i++){
		if(temp&0x80){
			shift1BitB(indicator);
		}else{
			shift0BitB(indicator);
		}
		temp = temp<< 1;
	}
	moveDataB(indicator);
}

void clearLED(int indicator){
	clearMemoryB(indicator);
	memoryOnB(indicator);
	shift0BitB(indicator);
	moveDataB(indicator);
}

//shared variables
int row[8] = {0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x87};
int row2[8] = {0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F};
	
int col[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
int gameOn = 0;//0
int ballOn = 0;
int gameOver = 0;
int currentCol = 0;//0
int displayStart = 0;//0
int start[8][15] = {
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD5,0xD5,0xC1,0xFF,0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF,0xFF,0xC5,0xD5,0xD1,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0x00,0x00,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xFD,0xFD,0xFC,0xFD,0xFD,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xFB,0xFB,0xF8,0xFB,0xFB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xF7,0xF7,0xF0,0xF7,0xF7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xEF,0xEE,0xE0,0xEF,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xDF,0xDD,0xC1,0xDF,0xDF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		
};


int scrollNum = 0;
int vScroll = 0;
int k = 0;
//int scrollDown[8] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
int displaying=0;

unsigned char D4 = 0x00;
enum sButton{startDisplay}startState;
int StartTick(int state){
	D4 = ~PIND&0x10;
	switch(state){
		case startDisplay:
			if(D4 && (!displaying)){
				displayStart = 1;
				displaying = 1;
				for(int i=0;i<8;i++){
					row[i] = row2[i];//just being lazy xd
				}
			}
			state = startDisplay;
			break;
		default:
			break;
	}
	return state;
};

enum drawStart{drawS}sState;
int sTick(int state){
	if(displayStart){
		switch(state){
			case drawS:
			if(scrollNum==0){
				shiftB(col[currentCol],0);
				shiftB(start[scrollNum][currentCol+vScroll],1);
				currentCol++;
				if(currentCol==8){
					currentCol=0;
					k++;
					if(k==30){
						vScroll++;
						k=0;
					}
				}
				if(vScroll == 7){
					scrollNum++;
					vScroll = 6;
				}
			}else if(scrollNum == 0x01){
				shiftB(col[currentCol],0);
				shiftB(start[scrollNum][currentCol+vScroll],1);
				currentCol++;
				if(currentCol==8){
					currentCol=0;
					k++;
					if(k==30){
						vScroll--;
						k=0;
					}
				}
				if(vScroll == 0){
					scrollNum ++;
					vScroll = 0;
				}
			}else{
				shiftB(col[currentCol],0);
				shiftB(start[scrollNum][currentCol],1);
				currentCol++;
				if(currentCol==8){
					currentCol=0;
					k++;
					if(k==30){
						scrollNum++;
						k=0;
					}
				}
				if(scrollNum == 8){
					scrollNum = 0;
					//vScroll = 0;
					displayStart = 0;
					gameOn = 1;
					gameOver = 0;
					//ballOn =1;
				}
				//scrollNum = 0;
			}
			state = drawS;
			break;
			default:
			state = drawS;
			break;
			
		}
	}
	return state;
};

enum drawMap{draw}mState;
int mTick(int state){
	if(gameOn){
		switch(state){
			case draw:
				shiftB(col[currentCol],0);
				shiftB(row[currentCol],1);
				currentCol++;
				if(currentCol==8){
					currentCol=0;
				}
				state = draw;
				break;
			default:
				state = draw;
				break;
		}
	}
	return state;
};

int p1First = 0x00;
int p2First = 0x00;

unsigned char D0 = 0x00;
unsigned char D1 = 0x00;

enum control1{wait1, up1, down1, waitHold1}c1State;
int c1Tick(int state){
	D0 = ~PIND&0x01;
	D1 = ~PIND&0x02;
	if(gameOn&&!gameOver){
		if(D0||D1){
			if(!p2First){
				p1First = 1;
			}
			ballOn = 1;
		}
		switch(state){
			case wait1:
				if(D0&!D1){
					state = up1;
				}else if(!D0&&D1){
					state = down1;
				}else{
					state = wait1;
				}
				break;
			case up1:
				state = waitHold1;
				break;
			case down1:
				state = waitHold1;
				break;
			case waitHold1:
				if(!D0&&!D1){
					state = wait1;
				}else{
					state = waitHold1;
				}
				break;
			default:
				break;
		}
		switch(state){
			case wait1:
				break;
			case up1:
				if(!(row[0]&0x10) && ballOn){
					row[0]= (row[0]>>1)|0x80;
				}
				break;
			case down1:
				if(!(row[0]&0x08) && ballOn){
					row[0] = (row[0]<<1)|0x01;
				}
				break;
			case waitHold1:
				break;
			default:
				break;
		}
	}
	return state;
};

unsigned char D2 = 0x00;
unsigned char D3 = 0x00;

enum control2{wait2, up2, down2, waitHold2}c2State;
int c2Tick(int state){
	D2 = ~PIND&0x04;
	D3 = ~PIND&0x08;
	if(gameOn&&!gameOver){
		if(D2||D3){
			if(!p1First){
				p2First = 1;
			}
			ballOn = 1;
		}
		switch(state){
			case wait2:
			if(D2&&!D3){
				state = up2;
			}else if(!D2 && D3){
				state = down2;
			}else{
				state = wait2;
			}
			break;
			case up2:
			state = waitHold2;
			break;
			case down2:
			state = waitHold2;
			break;
			case waitHold2:
			if(!D2&&!D3){
				state = wait2;
			}else{
				state = waitHold2;
			}
			break;
			default:
			break;
		}
		switch(state){
			case wait2:
			break;
			case up2:
			if(!(row[7]&0x10) && ballOn){
				row[7]= (row[7]>>1)|0x80;
			}
			break;
			case down2:
			if(!(row[7]&0x08) && ballOn){
				row[7] = (row[7]<<1)|0x01;
			}
			break;
			case waitHold2:
			break;
			default:
			break;
		}
	}
	return state;
};

int ballCol = 0;
int ballMove = 0;
int nextMove = 0;
int horizontal = 0;
int vertical = 0;
int out = 0;
int flip=0;

enum ballState{wait3,p1Serve, p2Serve,move,vMove,stop,endG} BState;
int bTick(int state){
	if(ballOn){
		switch(state){
			case wait3:
				if(p1First){
					state = p1Serve;
				}else if(p2First){
					state = p2Serve;
				}else{
					state = wait3;
				}
				break;
			case p1Serve:
				ballCol = 3;
				ballMove = 2;
				horizontal = 1;
				row[ballCol] = 0xF7;
				state = move;
				break;
			case p2Serve:
				ballCol = 4;
				ballMove = 5;
				horizontal = 1;
				row[ballCol] = 0xF7;
				state = move;
				break;
			case move:
				if(ballMove < ballCol && horizontal){
					if(ballMove!=0){
						row[ballCol] = 0xFF;
						ballCol = ballMove;
						row[ballCol] = 0xF7;
						ballMove--;
					}else{
						if(!(row[ballMove]&0x08)){//hit bar left
							if(row[ballMove]&0x10){
								vertical = 0;
								state = vMove;
							}else if(row[ballMove]&0x04){
								vertical = 1;
								state = vMove;
							}
							/*
							if(row[ballMove] == 0x87){
								vertical = 1;
								state = vMove;
								ballOn = 0;///////
							}else if(row[ballMove] == 0xF0){
								vertical = 0;
								state = vMove;
								//ballOn = 0;///////
							}
							*/
							ballMove = 2;
						}else{
							row[ballCol] = 0xFF;
							row[ballMove] = row[ballMove]&0xF7;
							ballOn = 0;
						}
					}
				}else if(ballMove > ballCol && horizontal){
					if(ballMove!=7){
						row[ballCol] = 0xFF;
						ballCol = ballMove;
						row[ballCol] = 0xF7;
						ballMove++;
					}else{
						if(!(row[ballMove]&0x08)){//hit bar
							if(row[ballMove]&0x10){
								vertical = 0;
								state = vMove;
							}else if(row[ballMove]&0x04){
								vertical = 1;
								state = vMove;
							}
							/*
							if(row[ballMove] == 0x87){
								vertical = 1;
								state = vMove;
								ballOn = 0;///////
							}else if(row[ballMove] == 0xF0){
								vertical = 0;
								state = vMove;
								//ballOn = 0;///////
							}
							*/
							ballMove = 5;
							
						}else{
							row[ballCol] = 0xFF;
							row[ballMove] = row[ballMove]&0xF7;
							ballOn = 0;
						}
					}
				}
				break;
			case vMove:
				if(vertical){//up
					//if(~nextMove&0x01){
						//vertical = !vertical;
					//}else{
						nextMove = (row[ballCol]>>1) | 0x80;
					//}
				}else{//down
					//if(~nextMove&0x08){
						//vertical=!vertical;
					//}else{
						nextMove = (row[ballCol]<<1) | 0x01;
					//}
				}
				if( (~row[ballCol]&0x01 || ~row[ballCol]&0x80) && flip){ // flip the direction when hitting top or bottom
					vertical = !vertical;
					flip=0;
				}else{//check if hits bar left or right, set next position backward
					if(ballMove == 7){
						if(!(~nextMove&row[ballMove])){//hit right
							ballMove = 5;
						}else{// not hit, game over
							row[ballMove] = (row[ballMove]&nextMove);
							row[ballCol] = 0xFF;
							ballOn = 0;
							gameOver = 1;
							out = 1;
						}
					}else if(ballMove == 0){
						if(!(~nextMove&row[ballMove])){//hit left
							ballMove = 2;
						}else{// not hit, game over
							row[ballMove] = (row[ballMove]&nextMove);
							row[ballCol] = 0xFF;
							ballOn = 0;
							gameOver = 1;
							out = 1;
						}
					}
					if(!out){
						row[ballMove] = nextMove;
						row[ballCol] = 0xFF;
						if(ballMove < ballCol){
							ballCol = ballMove; //update ball position
							ballMove--;
						}else{
							ballCol = ballMove;
							ballMove++;
						}
						flip = 1;
					}
				}
				state = vMove;
				break;
			default:
				break;
		}
	}
	return state;
};

int main(void)
{
    /* Replace with your application code */
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x00; PORTD = 0xFF;
	//PORTB = 0x08;
	//MState = wait;
	
	
	unsigned long int SMTick1_time = 1;
	unsigned long int SMTick2_time = 5;
	unsigned long int SMTick3_time = 5;
	unsigned long int SMTick4_time = 2;
	unsigned long int SMTick5_time = 50;
	unsigned long int SMTick6_time = 50;
	
	unsigned long int calGCD = 1;
	calGCD = findGCD(calGCD,SMTick1_time);
	calGCD = findGCD(calGCD,SMTick2_time);
	calGCD = findGCD(calGCD,SMTick3_time);
	calGCD = findGCD(calGCD,SMTick4_time);
	calGCD = findGCD(calGCD,SMTick5_time);
	calGCD = findGCD(calGCD,SMTick6_time);
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = calGCD;
	
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_time/GCD;
	unsigned long int SMTick2_period = SMTick2_time/GCD;
	unsigned long int SMTick3_period = SMTick3_time/GCD;
	unsigned long int SMTick4_period = SMTick4_time/GCD;
	unsigned long int SMTick5_period = SMTick5_time/GCD;
	unsigned long int SMTick6_period = SMTick6_time/GCD;
	
	//Declare an array of tasks
	static task task1, task2, task3 , task4, task5, task6;
	task *tasks[] = { &task1, &task2, &task3 ,&task4 ,&task5 ,&task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
	task1.state = draw;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &mTick;
	
	//Task 2
	task2.state = wait1;
	task2.period = SMTick2_period;
	task2.elapsedTime = SMTick2_time;
	task2.TickFct = &c1Tick;
	
	//Task 3
	task3.state = wait2;
	task3.period = SMTick3_period;
	task3.elapsedTime = SMTick3_time;
	task3.TickFct = &c2Tick;
	
	//Task 4
	task4.state = drawS;
	task4.period = SMTick4_period;
	task4.elapsedTime = SMTick4_time;
	task4.TickFct = &sTick;
	
	//Task 5
	task5.state = startDisplay;
	task5.period = SMTick5_period;
	task5.elapsedTime = SMTick5_time;
	task5.TickFct = &StartTick;
	
	//Task 6
	task6.state = wait3;
	task6.period = SMTick6_period;
	task6.elapsedTime = SMTick6_time;
	task6.TickFct = &bTick;
	
	TimerSet(GCD);
	TimerOn();
	

	//unsigned short i;
	
	
	//TimerSet(2);
	//TimerOn();
	int i = 0;
    while (1) 
    {	
		for(i = 0; i < numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime +=1;
		}
		
		
		//shiftB(0x00,0);
		
		//PORTA = col[i];
		//PORTA = 0x01;
		//PORTB = 0xFD;
		while (!TimerFlag);
		TimerFlag = 0;
    }
}