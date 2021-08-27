/*
 * File:   main.c
 * Author: ragab
 *
 * Created on July 29, 2021, 8:25 AM
 */

#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)


// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 4000000

#include <xc.h>
#include <pic12f675.h>

/**************************MACROS****************************/
#define POTT 0  //POTT RES 
#define LED4 1  //LED4
#define LED3 2  //LED3
#define BUTT 3  //INPUT BUTT
#define LED2 4 //LED2
#define LED1 5 //LED1


#define OUTPUTBIT(A)     TRISIO &=~(1<<A);
#define INPUTBIT(A)      TRISIO |=(1<<A);

#define OUTPUTPORTC   TRISIO &=~((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4))
#define INPUTPORTC    TRISIO |=((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4)

#define SETBIT(A)        GPIO |=(1<<A)
#define CLEARBIT(A)      GPIO &=~(1<<A)

#define SETPORT         GPIO |=((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4))
#define CLEARPORT       GPIO &=~((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4))

#define SETBITONLY(A)    CLEARPORT;GPIO |=(1<<A)
#define CLEARBITONLY(A)  SETPORT;CLEARBIT(A)

#define TOGALEPORT       GPIO ^=((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4))
#define TOGALEPORTT(A , B )      GPIO ^=((1<<A)|(1<<B))
#define TOGALEBIT(A)      GPIO ^=(1<<A)

#define TOGALEPORTTWO(A , B  , C , D  )      GPIO ^=((1<<A)|(1<<B)) ; GPIO ^=((1<<C)|(1<<D))

/*************************CHECK PORT***********************************/
#define CHECKGPIO(A) (GPIO  & (1<< A))
/***********************************************************************/

/*****************************GLOBAL VARIABLE****************************/
#define NUMBEROFLOOP 5
unsigned short delay_val;
/*****************************************************************/


/****************************define flages************************************/
#define LIGHT 0
#define DARK  1

#define TOGLEONE   1
#define TOGLETWO   2
#define TOGLETHREE 3
#define TOGLEFOUR  4

#define NO_TOGALE         0
#define TOGLE_LED1        1
#define TOGLE_LED2        2
#define TOGLE_LED1_LED2   3
#define TOGLE_LED1_LED4   4



/*****************************************************************************/
/*****/
//#define CHECKDEF
/*****/
/*********************prototype function FOR LED FLASHER **********************/
void forwardBackStepByStep(unsigned char  flage);
void forwardBackStepByStepReverse(unsigned char  flage);
void togaleLeds(unsigned char flage , unsigned char fristTogaleOrNo );
void forwardBackwordLightOneOnly();
void forwardBackwordLightOneByOne();
void forwardBackwordLightOneOnlyAndRevese(unsigned  char );

void togale_lightTwoByTwo();
void my_delay_ms(unsigned short n);
/*****************************************************************/
/********************prototype function FOR ADC**************************/
//Define Channels
#define AN0  1<<0
#define AN1  1<<1
#define AN2  1<<2
#define AN3  1<<3


//Function Declarations
void InitADC(unsigned char);
unsigned int GetADCValue(unsigned char);
/***********************************************************************/

/*****************************************GPIO_INIT**********************/
void gpio_init();
/**************************************************************************/


/////////////////////////////////////////////////////////////////////////////////////////////
void main(void) {
    gpio_init();
    InitADC(AN0);
    while(1){
        forwardBackwordLightOneOnlyAndRevese(DARK);
        forwardBackwordLightOneOnlyAndRevese(LIGHT);
        forwardBackStepByStep(LIGHT);
        forwardBackStepByStep(DARK);
        forwardBackStepByStepReverse(LIGHT);
        forwardBackStepByStepReverse(DARK);
        togaleLeds(TOGLEONE , NO_TOGALE);
        togaleLeds(TOGLEONE , TOGLE_LED1_LED4);
        togaleLeds(TOGLETWO , TOGLE_LED1_LED2);
        togaleLeds(TOGLETWO , TOGLE_LED1_LED4);


        
    }

    return;
}





/***********************GPIO FUNCTION*******************************************/
void gpio_init(){
    ANSEL  = 0x00;       // Set ports as digital I/O, not analog input
	ADCON0 = 0x00;		 // Shut off the A/D Converter
	CMCON  = 0x07;		 // Shut off the Comparator
	VRCON  = 0x00;	     // Shut off the Voltage Reference
	TRISIO = 0x00;       // GP3 input, rest all output
	GPIO   = 0x00;       // Make all pins 0
    
    /** SET LEDS , BUTT , POTT  DIRECTION***/
    INPUTBIT(BUTT);
    INPUTBIT(POTT);
    OUTPUTBIT(LED1);
    OUTPUTBIT(LED2);
    OUTPUTBIT(LED3);
    OUTPUTBIT(LED4);

}

/*********************************************************************************/



/**************************************ADC FUNCTION*************************************/
void InitADC(unsigned char Channel)
{
	ANSEL   = 0x10;	     // Clear Pin selection bits
	ANSEL  |= Channel;   // Select Channel
	//TRISIO  = 0x10;      // GP3 input, rest all output
	TRISIO |= Channel;	 // Make selected channel pins input
	ADCON0 = 0x81;		 // Turn on the A/D Converter
	CMCON  = 0x07;		 // Shut off the Comparator, so that pins are available for ADC
	VRCON  = 0x00;	     // Shut off the Voltage Reference for Comparator
}


/*
 * Function Name: GetADCValue
 * Input(s) :     Channel name, it can be AN0, AN1, AN2 or AN3 only.
 *                Channel is selected according to the pin you want to use in
 *                the ADC conversion. For example, use AN0 for GP0 pin.
 *				  Similarly for GP1 pin use AN1 etc.
 * Output(s):     10 bit ADC value is read from the pin and returned.
 * Author:        M.Saeed Yasin   20-06-12
 */
unsigned int GetADCValue(unsigned char Channel)
{
	ADCON0 &= 0xf3;      // Clear Channel selection bits

	switch(Channel)
	{
		case AN0:	ADCON0 |= 0x00; break;      // Select GP0 pin as ADC input
		case AN1:	ADCON0 |= 0x04; break;      // Select GP1 pin as ADC input
		case AN2:	ADCON0 |= 0x08; break;      // Select GP2 pin as ADC input
		case AN3:	ADCON0 |= 0x0c; break;      // Select GP4 pin as ADC input

		default:	return 0; 					//Return error, wrong channel selected
	}
    
    __delay_ms(10);      // Time for Acqusition capacitor to charge up and show correct value

	GO_nDONE  = 1;		 // Enable Go/Done

	while(GO_nDONE);     //wait for conversion completion

	return ((ADRESH<<8)+ADRESL);   // Return 10 bit ADC value
}

/***************************************************************************************/


/*********************************LEDS FUNCTION*****************************************************/


/******delay********/
void my_delay_ms(unsigned short n) {
 while(n--) {
  __delay_ms(1);
 }
}

/*******************************************************************************/

void forwardBackStepByStep(unsigned char  flage){
    unsigned char count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =5 ; i>0 ;i--){
            if(i ==3) 
                i=2;
            switch(flage){
                case LIGHT: SETBITONLY(i);   break;
                case DARK : CLEARBITONLY(i); break;
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif            
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);

        }
        CLEARPORT;

    }
    count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =1 ; i<6 ;i++){
            if(i ==3) 
                i=4;
            switch(flage){
                case LIGHT: SETBITONLY(i);   break;
                case DARK : CLEARBITONLY(i); break;
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);
            
        }
        CLEARPORT;
        
    }  
    CLEARPORT;
}

void forwardBackStepByStepReverse(unsigned char  flage){
    unsigned char count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =5 ; i>0 ;i--){
            if(i ==3) 
                i=2;
            switch(flage){
                case LIGHT: SETBITONLY(i);   break;
                case DARK : CLEARBITONLY(i); break;
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif            
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);

        }
        
    
        for(unsigned char  i =1 ; i<6 ;i++){
            if(i ==3) 
                i=4;
            switch(flage){
                case LIGHT: SETBITONLY(i);   break;
                case DARK : CLEARBITONLY(i); break;
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif            
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);
            
        }
        
        
   }
    CLEARPORT;
    
}
void togaleLeds(unsigned char flage , unsigned char fristTogaleOrNo ){
    unsigned char count=NUMBEROFLOOP;
    switch(fristTogaleOrNo){
        case NO_TOGALE       : CLEARPORT      ;            break;
        case TOGLE_LED1      : TOGALEBIT(LED1);            break;
        case TOGLE_LED2      : TOGALEBIT(LED2);            break;
        case TOGLE_LED1_LED2 : TOGALEPORTT(LED1 , LED2);   break;
        case TOGLE_LED1_LED4 : TOGALEPORTT(LED1 , LED4);   break;
    }
    while(count--){
        for(unsigned char  i =5 ; i>0 ;i--){
            if(i ==3) 
                i=2;
            switch(flage){
                case TOGLEONE  : TOGALEBIT(i);   break;
                case TOGLETWO  : TOGALEPORTTWO(LED1 ,LED2  , LED3 , LED4 );  break;
                case TOGLEFOUR : TOGALEPORT;   break;
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);

        }
       // CLEARPORT;

    }
    count=NUMBEROFLOOP;
//    switch(fristTogaleOrNo){
//        case NO_TOGALE       : CLEARPORT      ;            break;
//        case TOGLE_LED1      : TOGALEBIT(LED1);            break;
//        case TOGLE_LED2      : TOGALEBIT(LED2);            break;
//        case TOGLE_LED1_LED2 : TOGALEPORTT(LED1 , LED2);   break;
//        case TOGLE_LED1_LED4 : TOGALEPORTT(LED1 , LED4);   break;
//    }
    while(count--){
        for(unsigned char  i =1 ; i<6 ;i++){
            if(i ==3) 
                i=4;
            switch(flage){
                case TOGLEONE  : TOGALEBIT(i);   break;
                case TOGLETWO  : TOGALEPORTTWO(LED1 ,LED2  , LED3 , LED4 );  break;
                case TOGLEFOUR : TOGALEPORT;   break;

            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif          
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);
            
        }
        //CLEARPORT;
        
    }  
    CLEARPORT;
    
}

void forwardBackwordLightOneOnly(){
    unsigned char count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =5 ; i>0 ;i--){
            if(i ==3) 
                i=2;
            SETBITONLY(i);
            
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);

        }
        
    }
    count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =1 ; i<6 ;i++){
            if(i ==3) 
                i=4;
            SETBITONLY(i);
            
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);
            
        }
        
    }  
    CLEARPORT;
}

void forwardBackwordLightOneByOne(){
    unsigned char count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =5 ; i>0 ;i--){
            if(i ==3) 
                i=2;
            SETBIT(i);
            
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);

        }
        CLEARPORT;

    }
    count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =1 ; i<6 ;i++){
            if(i ==3) 
                i=4;
            SETBIT(i);
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);
            
        }
        CLEARPORT;
        
    }  
    CLEARPORT;
}
void forwardBackwordLightOneOnlyAndRevese(unsigned char flage){
    unsigned char count=NUMBEROFLOOP;
    while(count--){
        for(unsigned char  i =5 ; i>0 ;i--){
            if(i ==3) 
                i=2;
            if(flage){
                SETBITONLY(i);
            }else{
                CLEARBITONLY(i);
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);

        }
        
    
        for(unsigned char  i =1 ; i<6 ;i++){
            if(i ==3) 
                i=4;
            if(flage){
                SETBITONLY(i);
            }else{
                CLEARBITONLY(i);
            }
#ifdef CHECKDEF
            while(!CHECKGPIO(BUTT)){
            SETPORT;
            }            
#endif
            delay_val=GetADCValue(AN0);
            my_delay_ms(delay_val);
            
        }
        
        
   }
    CLEARPORT;
}


