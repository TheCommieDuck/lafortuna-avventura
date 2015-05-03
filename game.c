#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "game.h"

#define OUTSIDE 0
#define SD1 1
#define CORRIDOR_1 2
#define CORRIDOR_2 3
#define IC1 4
#define IC2 5
#define IC3 6
#define IC4 7
#define RESET 8
#define AVCC 9
#define LED_ROOM 10
#define USB 11
#define SCROLL_CORR 12
#define SCROLL_HUB 13
#define EXT_PIN_1 14
#define CORRIDOR_3 15
#define ROTARY 16
#define BATT 17
#define CAP_BANK 18
#define EXT_PIN_2 19
#define CORRIDOR_FINAL 20
#define LCD_SCREEN_ROOM 21
#define OUTSIDE_2 22

#define IC_FLAVOUR "You are standing in a large, spacious room. This seems to be the main unit of the board."

#define NO_ROOM 255

#define DIR_NORTH 0
#define DIR_SOUTH 1
#define DIR_WEST 2
#define DIR_EAST 3

#define EVERY_TURN 0
#define DESCRIPTION 1
#define LED_SWITCH 2

int player_pos = OUTSIDE;

command commands[NUM_COMMANDS] = {
	{"north", north, 0}, 
	{"south", south, 0},
	{"east", east, 0},
	{"west", west, 0},
	{"look", look, 0},
	{"examine", examine, 1},
	{"use", use, 1},
	{"eat", eat, 1},
	{"open", open, 1},
	{"close", close, 1}};
 
command* active_commands = commands;

void die()
{
	display_string("You have died. RIP!\nReset the board to play again.");
	while(1){};
}

void outside_room(void* r, uint8_t action)
{
	static uint8_t countdown = 3;

	switch(action)
	{
		case EVERY_TURN:
		if(countdown == 3)
			display_string("It's very hot out here. You should get inside before you hurt yourself from this heat.\n");
		else if(countdown == 2)
			display_string("You start to feel woozy in the heat. You're starting to get sunstroke.\n");
		else if(countdown == 1)
			display_string("You're on the verge of passing out. Get inside, quick!\n");
		else
		{
			display_string("You collapse from the heat. You are eaten by a passing sun-grue.\n");
			die();
		}
		countdown--;
		break;
		default:
		break;
	}
}

void above_scroll_room(void* r, uint8_t action)
{
	switch(action)
	{
		case EVERY_TURN:
		break;
		default:
		break;
	}
}

void led_room(void* r, uint8_t action)
{
	static uint8_t led_is_on = 0;
	switch(action)
	{
		case LED_SWITCH:
		if(led_is_on)
		{
			display_string("You turn the LED off.\n");
			led_is_on = 0;
		}
		else
		{
			display_string("You turn the LED on.\n");
			led_is_on = 1;
		}
		break;
		case EVERY_TURN:
		break;
		case DESCRIPTION:
		if(led_is_on)
			((room*)r)->desc = "You look down. You are standing on top of the LED. It is currently on, and the room is very bright.";
		else
		{
			((room*)r)->desc = 
		"You look down. You are standing on top of the LED. It is currently off, and the room is dark. The floor hums.";
		}
		break;
		default:
		break;
	}

}

void final_room(void* r, uint8_t action)
{
	switch(action)
	{
		case EVERY_TURN:
		break;
		default:
		break;
	}

}

void ded_room(room* r, uint8_t action)
{
	switch(action)
	{
		case EVERY_TURN:
		die();
		break;
		default:
		break;
	}
}

room rooms[NUM_ROOMS] = {
	{"Outside", 
	"You are outside the LaFortuna. It is really, really hot out here.", {NO_ROOM, NO_ROOM, SD1, NO_ROOM}, outside_room},
	{"SD Card Slot", "You are standing in the SD card slot.", {NO_ROOM, NO_ROOM, CORRIDOR_1, OUTSIDE}, 0},
	{"A Corridor", "You find yourself in a dimly lit corridor. The walls are lined with pulsing green lights.", 
	{NO_ROOM, NO_ROOM, CORRIDOR_2, SD1}, 0},
	{"A Corridor", "The corridor seems darker now. The lights are all off.", {NO_ROOM, NO_ROOM, IC1, CORRIDOR_1}, 0},
	{"Integrated Circuit Unit, North Side", IC_FLAVOUR, {NO_ROOM, IC2, RESET, CORRIDOR_2}, 0},
	{"Integrated Circuit Unit, North-Central Side", IC_FLAVOUR, {IC1, IC3, AVCC, LED_ROOM}, 0},
	{"Integrated Circuit Unit, South-Central Side", IC_FLAVOUR, {IC2, IC4, USB, SCROLL_CORR}, 0},
	{"Integrated Circuit Unit, South Side", IC_FLAVOUR, {IC3, BATT, EXT_PIN_1, CORRIDOR_3}, 0},
	{"Reset Button", "Above you, you can see the reset switch. You could probably just about reach it from here.",
	 {NO_ROOM, NO_ROOM, NO_ROOM, IC1}, 0},
	{"AVCC Switch Room", "The walls pulse around you. Everything is oscillating wildly in here. It's giving you a headache.", 
	{NO_ROOM, NO_ROOM, NO_ROOM, IC2}, 0},
	{"LED Room", "", 
	{NO_ROOM, NO_ROOM, IC2, SD1}, led_room},
	{"USB Port", 
	"As you walk to the edge of the room, you look out the USB socket. It's a long, long way down. You'd probably die if you fell out.", 
	{NO_ROOM, NO_ROOM, OUTSIDE_2, IC3}, 0},
	{"Above Scroll Wheel", "You find yourself on a silicon catwalk. Below you is a circular room, split into 5 sections.", 
	{NO_ROOM, NO_ROOM, IC3, NO_ROOM}, above_scroll_room},
	{"Scroll Wheel", 
		"You climb down the ladder and step on the floor of the circular room. The floor seems to depress when you stand on it.", 
		{NO_ROOM, NO_ROOM, NO_ROOM, NO_ROOM}, 0},
	{"GPIO Pins", 
	"There are a number of holes above you, light streaming in.", {NO_ROOM, NO_ROOM, NO_ROOM, IC4}, 0},
	{"A Corridor", "You find yourself in a dimly lit corridor. The walls are lined with pulsing green lights.", 
		{EXT_PIN_2, ROTARY, IC4, CORRIDOR_FINAL}, 0},
	{"Rotary Encoder Control Room", "The room is spinning round and round and round and round...", {CORRIDOR_3, NO_ROOM, NO_ROOM, NO_ROOM}, 0},
	{"Battery Room Place", "Battery stuff.", {IC4, NO_ROOM, NO_ROOM, CAP_BANK}, 0},
	{"Capacitor Bank", "CAPACITORS ERRYWHERE.\n", {NO_ROOM, NO_ROOM, BATT, CAP_BANK}, 0},
	{"GPIO Pins", "There are a number of holes above you, light streaming in.\n", 
	{NO_ROOM, CORRIDOR_3, NO_ROOM, NO_ROOM}, 0},
	{"An Ominous Corridor", 
	"As you walk down the corridor, you have a very ominous feeling. Almost like something very bad is at the end of the corridor.", 
	{NO_ROOM, NO_ROOM, CORRIDOR_3, NO_ROOM}, 0},
	{"The LCD Screen", 
	"As you walk into the LCD screen, a cold breeze brushes over you. A giant dragon flies down from the ceiling and lands in front of you.", 
	{NO_ROOM, NO_ROOM, CORRIDOR_FINAL, NO_ROOM}, final_room},
	{"Outside, Falling", 
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-splat. I told you it was a long way down.", 
	{NO_ROOM, NO_ROOM, NO_ROOM, NO_ROOM}, ded_room}
};

void print_player_pos()
{
	display_string(rooms[player_pos].name);
	display_string("\n");
	if(rooms[player_pos].update != 0)
	{
		display_string("\n");
		rooms[player_pos].update((void*)(&rooms[player_pos]), DESCRIPTION);
	}
	display_string(rooms[player_pos].desc);
	display_string("\n");
	uint8_t i = 0;
	for(;i < 4; ++i)
	{
		if(rooms[player_pos].connections[i] == NO_ROOM)
			continue;
		display_string("To the ");
		display_string(i == DIR_NORTH ? "north" : i == DIR_SOUTH ? "south" : i == DIR_WEST ? "west" : "east");
		display_string(" you can go to the ");
		display_string(rooms[rooms[player_pos].connections[i]].name);
		display_string(".\n");
	}
}

void init_game()
{
	print_player_pos();
}

void do_command(int selection)
{
	commands[selection].command();
	if(rooms[player_pos].update != 0)
	{
		display_string("\n");
		rooms[player_pos].update((void*)(&rooms[player_pos]), EVERY_TURN);
	}
}

void go(uint8_t dir)
{
	if(rooms[player_pos].connections[dir] == NO_ROOM)
	{
		display_string("You can't go that way.");
		return;
	}
	else
	{
		player_pos = rooms[player_pos].connections[dir];
		print_player_pos();
	}
}

void north()
{
	go(DIR_NORTH);
}
void south()
{
	go(DIR_SOUTH);
}
void east()
{
	go(DIR_EAST);
}
void west()
{
	go(DIR_WEST);
} 

void look()
{
	print_player_pos();
}
void examine()
{

}
void use()
{
	if(player_pos == LED_ROOM)
		rooms[LED_ROOM].update((void*)(&rooms[LED_ROOM]), LED_SWITCH);
}
void eat()
{

}
void open()
{

}
void close()
{

}