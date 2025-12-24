#include <stdlib.h>
#include <time.h>
//#include <conio.h>
#include <string.h>
#include <peekpoke.h>
#define byte unsigned char
#include "sprites.h"

#define STAY		0
#define MOVE		1
#define SELECT		2
#define SLEEP		3
#define TELEPORT	4
#define BUMP		5

#define LEFT	1
#define RIGHT	2
#define UP		3
#define DOWN	4

struct sprite {
	byte color;
    unsigned int frame_pointer;
    byte cur_frame;
    unsigned int pos;
    unsigned int origpos;
    byte steps;
    byte gridpos;
    byte movedir;
    byte action;
    byte* walk_left_ani;
    byte* walk_right_ani;
    byte* walk_down_ani;
    byte* walk_up_ani;
    byte* stay_ani;
    byte* sleep_ani;
};

struct sprite *player;
struct sprite skweek;
byte select_cntr = 0;
byte spr_bwidth = 3; // how many bytes wide (groups of 6 pixels)
byte spr_height = 18; // how many lines
byte last_frame = 2;
unsigned int frame_offset = 54;
unsigned int last_frame_offset  = 108;
byte *chrs = (byte *)0x9900;
unsigned int draw_saddr = 0xa3c0;  // start address of game draw area
byte need_cleanup = 0;
byte need_cdclean = 0;
byte* clean_spr;
byte downmap[3] = { 2, 0, 1 };
byte upmap[3] = { 0, 2, 1 };

#include "draw.h"
#include "move.h"

unsigned char __near__ GetKey (unsigned char code);

void setup() {
    if(PEEK(0xC800))
            asm("jsr $EC33");       // HIRES in Atmos (ROM 1.1)
    else
            asm("jsr $E9BB");       // HIRES in Oric-1 (ROM 1.0)
    POKE(0x24E, 5);                 // set keyb delay 5 at #24E
    POKE(0x26A, 10);                // disable keyb click and caret blink with bitflag at #26A
	memset((char*)0xBF68, ' ', 200);  // Clear lower text area
}

void init_aicCY() {
	int i;
	byte c = 0;
	for(i=0xa000; i<0xbf3f; i=i+40) { // setup AIC mode cyan/yellow
		if(c==0) { POKE(i,6); c=1;
		} else { POKE(i,3); c = 0; }
	}
}

void clear_screen() {
	int i;
	int addr = 0xa001;
	for(i=0;i<200;i++) {
		memset((char*)addr,64,39);
		addr = addr + 40;
	}
}

void wait_centis(int centisecs) {
	clock_t clk = clock();
	while(clock()-clk < centisecs) {
		clock();
	}
}

void init_player() {
	skweek.pos = 5;
	skweek.steps = 0;
	skweek.action = STAY;
	skweek.movedir = STAY;
	skweek.walk_left_ani = blue_walk_left;
	skweek.walk_right_ani = blue_walk_right;
	skweek.walk_down_ani = blue_walk_down;
	skweek.walk_up_ani = blue_walk_up;
	skweek.stay_ani = blue_stay;
	skweek.sleep_ani = blue_sleep;
	skweek.color = 0;
	skweek.gridpos = 10;
	skweek.origpos = 10;
	skweek.pos = 10;
}

void animate_sprite(struct sprite *spr) {
	// draw
	if(spr->action==STAY || spr->action==SLEEP) {
		select_cntr = 0;
		draw_spr(spr->stay_ani,draw_saddr+spr->pos);
	}
	
	if(spr->action==SELECT) {
		if(select_cntr<7) {
			draw_selected(spr->stay_ani,draw_saddr+spr->pos);
			select_cntr = select_cntr + 1;
		} else {
			draw_spr(spr->stay_ani,draw_saddr+spr->pos);
			spr->action = STAY;
			select_cntr = 0;
		}
		// debug: 
		//puttext(0xBFB9,itoa(instruments[0].snd_time, buf, 10));
	}
	
	if(spr->action==MOVE) {
		switch(spr->movedir) {
			case LEFT:	move_left(player);
						if(need_cleanup) { draw_spr_col(clean_spr, 2-spr->steps, draw_saddr+spr->pos+3); need_cleanup = 0; }
						draw_spr(spr->walk_left_ani+spr->frame_pointer,draw_saddr+spr->pos); break;
			case RIGHT: move_right(player);
						if(need_cleanup) { draw_spr_col(clean_spr, spr->steps, draw_saddr+spr->pos-1); need_cleanup = 0; }
						draw_spr(spr->walk_right_ani+spr->frame_pointer,draw_saddr+spr->pos); break;
			case DOWN: 	move_down(player);
						if(need_cdclean) { draw_spr_rows(clean_spr, 3, 0, draw_saddr+spr->origpos); need_cdclean = 0; }
						if(need_cleanup) { draw_spr_rows(clean_spr, 3, downmap[spr->steps], draw_saddr+spr->origpos); need_cleanup = 0; }
						draw_spr(spr->walk_down_ani+spr->frame_pointer,draw_saddr+spr->pos); break;
			case UP: 	move_up(player);
						if(need_cdclean) { draw_spr_rows(clean_spr, 3, 2, draw_saddr+spr->origpos); need_cdclean = 0; }
						if(need_cleanup) { draw_spr_rows(clean_spr, 3, upmap[spr->steps], draw_saddr+spr->origpos); need_cleanup = 0; }
						draw_spr(spr->walk_up_ani+spr->frame_pointer,draw_saddr+spr->pos); break;
		}
	}
}

void main() {
    clock_t interval = CLOCKS_PER_SEC/30;
    clock_t prevaclk = 0;
	setup();
	clear_screen();
	init_aicCY();
	init_player();
	player = &skweek;
	clean_spr = wall1;
	while(1) {
		if(player->action==STAY || player->action==SLEEP) {
			if(GetKey(15)/*d*/) {
				player->movedir = RIGHT; 
				player->action = MOVE;
			}
			if(GetKey(53)/*a*/) {
				player->movedir = LEFT;
				player->action = MOVE;
			}
			if(GetKey(54)/*s*/) {
				player->movedir = DOWN;
				player->action = MOVE; need_cdclean = 1;
			}
			if(GetKey(55)/*w*/) {
				player->movedir = UP;
				player->action = MOVE; need_cdclean = 1;
			}
			if(GetKey(61)/*Return*/) {
				player->action = SELECT;
                player->steps = 0;
			}
			if(GetKey(13)/*Esc*/) { exit(0); }
			wait_centis(1);
		}
		if(clock()-prevaclk > interval) {
			animate_sprite(player);
			prevaclk = clock();
		}
	}
}
