#include <stdio.h>
#include <wiringPi.h>

const char three_bit_bus[] = {0, 2, 3}; //BCM 17, 27 and 22 for a 3-bit parallel three_bit_bus we use to send a number to the board. 4, 2, 1 in order.
const char FF_input = 4; //BCM 23 for flipflop input.
const char FF_output = 5; //BCM 24 for flipflop output to reset.
const int max_delay = 600;
int total_score = 0;

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
	delay(max_delay / current_light); //Delay our input check based on our current light. If the light is #4 our delay is the smallest.
	if (!digitalRead(FF_input)) { //On input from flipflop we count what score the player has:
		total_score += current_light; //The number of the light also depicts its score, so we add it to our total score.
		printf("Current score is: %d\nTotal score is: %d\n", current_score, total_score);
		digitalWrite(FF_output, HIGH); //Reset the flipflop.
		flash_light(current_light); //Flash light in the meanwhile to indicate during which light you shot.
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
	//INIT
	wiringPiSetup();
	for (int bit = 0; bit < 3; bit++) {
		pinMode(three_bit_bus[bit], OUTPUT);
	}
	pinMode(FF_input, INPUT);
	pinMode(FF_output, OUTPUT);

	printf("Game start!\n");
	while(1) {
		for (char i = 1; i < 5; i++) {
			send_bits(i);
			if(player_has_shot(i))
				break;
		}
		for (char i = 4; i > 1; i--) { //We skip i=1 because our previous loop starts with it.
			send_bits(i);
			if(player_has_shot(i))
				break;
		}
	}
	return 0;
}
