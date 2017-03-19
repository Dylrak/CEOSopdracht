#include <stdio.h>
#include <wiringPi.h>

const char three_bit_bus[] = {0, 2, 3}; //BCM 17, 27 and 22 for a 3-bit parallel three_bit_bus we use to send a number to the board. 4, 2, 1 in order.
const char FF_input = 4; //BCM 23 for flipflop input.
const char FF_output = 5; //BCM 24 for flipflop output to reset.
const int max_delay = 300;
int score = 0;

int get_bit(unsigned char ch, int n) {//n=0 is right-most bit.
	unsigned char mask=1<<n;
	if ((ch & mask)>>n) {
		return HIGH;
	} else {
		return LOW;
	}
}

void flash_light(char current_light) {
	char all_off = 0;
	for (int i = 0; i < 3; i++) { //Flash the light three times.
		send_bits(all_off);
		delay(max_delay);
		send_bits(current_light);
		delay(max_delay);
	}
}

int player_has_shot(char current_light) { //Returns true if we get input from the flipflop.
	int current_delay; //We start by calculating the delay based on what light we are on. The closer to the centre, the smaller the delay.
	if (current_light > 4) { //If the current number is higher than the middle light we start counting backwards.
		current_delay = max_delay / (current_light - 2 * (current_light - 4)); //So light 5 = score 3, 6 = 2, 7 = 1.
	} else { //Else, the score is our current number.
		current_delay = max_delay / current_light;
	}
	delay(current_delay);
	if (!digitalRead(FF_input)) { //On input from flipflop we count what score the player has:
		score += max_delay / current_delay;
		printf("Score is: %d\n", score); //Print it,
		digitalWrite(FF_output, HIGH); //And reset the flipflop.
		flash_light(current_light);
		digitalWrite(FF_output, LOW);
		return 1;
	}
	return 0;
}

void send_bits(char current_light) { //Current_light is the bits representing what light will be on.
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

	printf("Game start!\n");
	while(1) {
		for (char i = 1; i < 8; i++) {
			send_bits(i);
			if(player_has_shot(i))
				printf("Current score is: %d\n", score);
				break;
		}
		for (char i = 7; i > 0; i--) {
			send_bits(i);
			if(player_has_shot(i))
				printf("Current score is: %d\n", score);
				delay(max_delay);
				break;
		}
	}
	return 0;
}
