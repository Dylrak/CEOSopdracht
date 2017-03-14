#include <stdio.h>
#include <wiringPi.h>

int get_bit(unsigned char ch, int n) {//Index is 1-based, not zero
	n = 8 - n;
	unsigned char mask=1<<n;
	if ((ch & mask)>>n) {
		return HIGH;
	} else {
		return LOW;
	}
}
int main(int argc, char **argv)
{
	wiringPiSetup();
	pinMode(5, OUTPUT); //4
	pinMode(6, OUTPUT); //2
	pinMode(13, OUTPUT);//1. We use 5,6 and 13 as parallel bus.
	pinMode(19, INPUT);
	pinMode(26, OUTPUT);
	
	unsigned char current_number = 1;

	for (int i = 1; i < 5; i++) {
		printf("%d", get_bit(current_number, 0));
		digitalWrite (13, get_bit(current_number, 0));
	}
	return 0;
}
