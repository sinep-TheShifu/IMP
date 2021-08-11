/* Author : Daniel Andrasko
 * Login: xandra05
 * Project: IMP - Svetelna tabule
 * File: Header file for main
 * */

/* Variables */
int type;
int counter = 0;
int cyclecounter = 0;
int pressed_up = 0, pressed_down = 0;
unsigned int compare = 0x200;

/* Output matrixes */
int imp[8][16] =
{
		{1,1,1,1,1,0,1,0,0,0,1,0,1,1,1,0},
		{0,0,1,0,0,0,1,1,0,1,1,0,1,0,1,0},
		{0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0},
		{0,0,1,0,0,0,1,0,0,0,1,0,1,1,1,0},
		{0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0},
		{0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0},
		{0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0},
		{1,1,1,1,1,0,1,0,0,0,1,0,1,0,0,0}
};

int fit[8][16] =
{
		{1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,0},
		{1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,0},
		{1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0},
		{1,1,1,1,0,0,1,1,0,0,0,1,1,0,0,0},
		{1,1,1,1,0,0,1,1,0,0,0,1,1,0,0,0},
		{1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0},
		{1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0},
		{1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0}
};

int abc[8][16] =
{
		{1,1,1,0,0,1,1,1,0,0,0,1,1,0,0,0},
		{1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0},
		{1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0},
		{1,1,1,0,0,1,1,1,0,0,1,0,0,1,0,0},
		{1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0},
		{1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0},
		{1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0},
		{1,1,1,0,0,1,0,0,1,0,0,1,1,0,0,0}
};

int msg[8][16] =
{
		{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0},
		{0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0},
		{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
};

/* Definitions */
/* Constants specifying delay loop duration */
#define	tdelay1			1000
#define tdelay2 		20

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

/* Button masks */
#define BTN_SW2 0x400     // Port E, bit 10
#define BTN_SW3 0x1000    // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26
#define BTN_SW6 0x800     // Port E, bit 11

/* Functions */
void MCUInit(void);
void PortsInit(void);
void LPTMR0_IRQHandler(void);
void LPTMR0Init(int count);
void rowOff(unsigned int row);
void rowOn(unsigned int row);
void column_select(unsigned int col_num);
void delayLED(int t1, int t2);
void delay(long long bound);
void print(int matrix[8][16]);
void callPrint(int type);
