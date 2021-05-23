/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "S32K142.h" /* include peripheral declarations S32K142 */

float valor[10];
unsigned char j;
float constapeso = 0.0000100043;
float sumapeso_offset = 84.7903583;
long offset = 0;

void delay(void) {
	LPTMR0 -> CMR = 1;
	LPTMR0 -> PSR = (2<<3) + (1<<2);
	LPTMR0 -> CSR = 1;
	while (((LPTMR0 -> CSR) & (1 << 7)) == 0);
	LPTMR0 -> CSR = 0;

}

unsigned long ReadCount(void){
	unsigned long Count;
	unsigned int i;

	PTB -> PCOR = (1<<15);									//Apagar clock
	delay();
	Count=0;

	while((PTB -> PDIR & (1<<14)) == (1<<14));

	for (i=0;i<24;i++){
		PTB -> PSOR = (1<<15);								//Prender Clock
		delay();
		Count=Count<<1;
		PTB -> PCOR = (1<<15);								//Apagar clock
		delay();
		if((PTB -> PDIR & (1<<14)) == (1<<14)) Count++;
	}

	PTB -> PSOR = (1<<15);									//Prender Clock
	delay();
	Count=Count^0x800000;
	PTB -> PCOR = (1<<15);									//Apagar clock
	delay();

	return(Count);
}

unsigned long ReadAverage(int times){
	unsigned long sum = 0;
	unsigned int i;
	for (i = 0; i < times; i++) {
		sum += ReadCount();
	}
	return sum / times;
}

double GetValue(int times){
	return ReadAverage(times) - offset;
}

float GetUnits(int times){
	return -(constapeso * GetValue(times)) + sumapeso_offset;
}

void tare(int times){
	double sum = ReadAverage(times);
	offset = sum;
}

int main(void)
{
	SCG -> SIRCDIV = (1<<8);
	PCC -> PCCn[PCC_PORTB_INDEX] = (1<<30);        			//Habilitar Reloj Puerto C, solo se habilita CGC bit 30 (C29-631)
	PORTB -> PCR[14] = (1<<8);								//Habilitar Puerto C GPIO DATA
	PORTB -> PCR[15] = (1<<8);								//Habilitar Puerto C GPIO CLOCK

	PCC -> PCCn[PCC_LPTMR0_INDEX] = (1<<30) + (2<<24);


	PTB -> PDDR = (1<<15);                    	 			//Configurar pin 5 como salida clock

        for(;;) {
        	valor[(j++)%10] = GetUnits(20);
        }
	return 0;
}
