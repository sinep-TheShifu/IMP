/* Author : Daniel Andrasko
 * Login: xandra05
 * Project: IMP - Svetelna tabule
 * File: Main file
 * */

/* Header files */
#include "MK60D10.h"
#include "main.h"

int main(void)
{
    MCUInit();
    PortsInit();
    LPTMR0Init(compare);

    PTA->PDOR |= GPIO_PDOR_PDO(0x3F000280); // turning the pixels of a particular row ON
    delayLED(tdelay1, tdelay2);				// delay

    while (1)
    {
        if (!pressed_up && !(GPIOE_PDIR & BTN_SW2))
        {
        	type = 0;
            pressed_up = 1;
            compare -= 0x40;
        }
        else if (GPIOE_PDIR & BTN_SW2) pressed_up = 0;

        if (!pressed_down && !(GPIOE_PDIR & BTN_SW3))
        {
        	type = 1;
            pressed_down = 1;
            compare += 0x40;
        }
        else if (GPIOE_PDIR & BTN_SW3) pressed_down = 0;

        if (!pressed_up && !(GPIOE_PDIR & BTN_SW4))
        {
        	type = 2;
        	pressed_up = 1;
        	compare -= 0x40;
        }
        else if (GPIOE_PDIR & BTN_SW4) pressed_up = 0;

        if (!pressed_up && !(GPIOE_PDIR & BTN_SW5))
        {
        	type = 3;
        	pressed_up = 1;
        	compare -= 0x40;
        }
        else if (GPIOE_PDIR & BTN_SW5) pressed_up = 0;

        if (compare < 0x40) compare = 0x40;
        if (compare > 0x400) compare = 0x400;

        /* Turning the lights on  */
        callPrint(type);
    }

    return 0;
}

/* A delay function */
void delay(long long bound)
{

  long long i;
  for(i=0;i<bound;i++);
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/* Ports initialize */
void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK;

    /* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
    PORTA->PCR[8]  = (0|PORT_PCR_MUX(0x01));  // A0
    PORTA->PCR[10] = (0|PORT_PCR_MUX(0x01));  // A1
    PORTA->PCR[6]  = (0|PORT_PCR_MUX(0x01));  // A2
    PORTA->PCR[11] = (0|PORT_PCR_MUX(0x01));  // A3

    /* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
    PORTA->PCR[26] = (0|PORT_PCR_MUX(0x01));  // R0
    PORTA->PCR[24] = (0|PORT_PCR_MUX(0x01));  // R1
    PORTA->PCR[9]  = (0|PORT_PCR_MUX(0x01));  // R2
    PORTA->PCR[25] = (0|PORT_PCR_MUX(0x01));  // R3
    PORTA->PCR[28] = (0|PORT_PCR_MUX(0x01));  // R4
    PORTA->PCR[7]  = (0|PORT_PCR_MUX(0x01));  // R5
    PORTA->PCR[27] = (0|PORT_PCR_MUX(0x01));  // R6
    PORTA->PCR[29] = (0|PORT_PCR_MUX(0x01));  // R7

    /* Set corresponding PTB pins (connected to LED's) for GPIO functionality */
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); // D9
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); // D10
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); // D11
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); // D12

    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    /* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
    PORTE->PCR[28] = (0|PORT_PCR_MUX(0x01)); // #EN

    /* Change corresponding PTA port pins as outputs */
    PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

    /* Change corresponding PTE port pins as outputs */
    PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );
}

/* LPTMR0 IRQHandler */
void LPTMR0_IRQHandler(void)
{
    // Set new compare value set by up/down buttons
    LPTMR0_CMR = compare;                // !! the CMR reg. may only be changed while TCF == 1
    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // writing 1 to TCF tclear the flag
}

/* LPTMR0 Initialize */
void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK; // Enable clock to LPTMR
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;   // Turn OFF LPTMR to perform setup
    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK   // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ;   // use the choice of clock
    LPTMR0_CMR = count;                  // Set compare value
    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK    // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK    // LPT interrupt enabled
                );
    NVIC_EnableIRQ(LPTMR0_IRQn);         // enable interrupts from LPTMR0
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;    // Turn ON LPTMR0 and start counting
}

/* Switch LED of a particular rows off */
void rowOff(unsigned int row)
{
	switch(row)
	{
		case 0:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
			break;
		case 1:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
			break;
		case 2:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
			break;
		case 3:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
			break;
		case 4:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
			break;
		case 5:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
			break;
		case 6:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
			break;
		case 7:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
			break;
	}
}

/* Switch LED of a particular rows on */
void rowOn(unsigned int row)
{
	switch(row)
	{
		case 0:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
			break;
		case 1:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
			break;
		case 2:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
			break;
		case 3:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
			break;
		case 4:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
			break;
		case 5:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
			break;
		case 6:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
			break;
		case 7:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
			break;
	}
}

/* Conversion of requested column number into the 4-to-16 decoder control.  */
void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];

	for (i =0; i<4; i++) {
		result = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num = result;

		switch(i) {

			// Selection signal A0
		    case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;

			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;

			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;

			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

/* Variable delay loop */
void delayLED(int t1, int t2)
{
	int i, j;

	for(i=0; i<t1; i++) {
		for(j=0; j<t2; j++);
	}
}

/* Turn the lights on */
void print(int matrix[8][16])
{
	for (int i = 15; i >= 0; i--)
	{
		for (int j = 0; j <= 7; j++)
		{
			if (matrix[j][i])
			{
				rowOn(j);
			}
			else
			{
				rowOff(j);
			}
		}

		column_select(i - counter);

		PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );
		delayLED(tdelay1, tdelay2);
		PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	}

	cyclecounter++;
	if (cyclecounter % 8 == 0)
	{
		counter++;
		cyclecounter = 1;
	}
}

/* Calls print with the selected matrix */
void callPrint(int type)
{
	if(type == 0)
	{
		print(imp);
	}
	else if(type == 1)
	{
		print(fit);
	}
	else if(type == 2)
	{
		print(abc);
	}
	else if(type == 3)
	{
		print(msg);
	}
}
