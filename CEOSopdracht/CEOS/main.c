#include <stdio.h>
#include <wiringPi.h>

const char three_bit_bus[] = {0, 2, 3}; //BCM 17, 27 and 22 for a 3-bit parallel three_bit_bus we use to send a number to the board. 4, 2, 1 in order.
const char FF_input = 4; //BCM 23 for flipflop input.
const char FF_output = 5; //BCM 24 for flipflop output to reset.
int score = 0;

int get_bit(unsigned char ch, int n) {//Index is 1-based, not zero. n=0 is right-most bit.
	unsigned char mask=1<<n;
	if ((ch & mask)>>n) {
		return HIGH;
	} else {
		return LOW;
	}
}

int check_score(char current_light) {
	if (!digitalRead(FF_input)) { //On input from flipflop we count what score the player has:
		if (current_light > 4) { //If the current number is higher than the middle light we start counting backwards.
			score += current_light - 2 * (current_light - 4); //So light 5 = score 3, 6 = 2, 7 = 1.
		} else { //Else, the score is our current number.
			score += current_light; 
		}
		printf("Score is: %d\n", score); //Print it,
		digitalWrite(FF_output, HIGH); //And reset the flipflop.
		delay(1000);
		digitalWrite(FF_output, LOW);
	}
}
int send_bits(char current_light) { //Current number represents what lights will be on.
	 //Otherwise we send the bits of our current number down the three bit bus.
	for (int bit = 0; bit < 3; bit++) {
		digitalWrite (three_bit_bus[bit], get_bit(current_light, bit));
	}
}

int main(int argc, char **argv)
{
	wiringPiSetup();
	for (int bit = 0; bit < 3; bit++) {
		pinMode(three_bit_bus[bit], OUTPUT);
	}
	pinMode(FF_input, INPUT);
	pinMode(FF_output, OUTPUT);

	printf("Game start!");
	while(1) {
		for (char i = 1; i < 8; i++) {
			send_bits(i);
			delay(100);
			check_score(i);
		}
		for (char i = 7; i > 0; i--) {
			send_bits(i);
			delay(100);
			check_score(i);
		}
	}
	return 0;
}
