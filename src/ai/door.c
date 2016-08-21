#include "ai.h"

#include <genesis.h>
#include "audio.h"
#include "player.h"
#include "stage.h"
#include "tables.h"
#include "tsc.h"

void ai_door_onCreate(Entity *e) {
	// When the door's direction is changed to be facing right it becomes transparent
	if(e->direction) e->spriteAnim = SPRITE_DISABLE;
	u16 x = sub_to_block(e->x), y = sub_to_block(e->y);
	if(stage_get_block_type(x, y + 1) != 0x41) { // Push down if too high
		e->y += block_to_sub(1);
	} else if(stage_get_block_type(x, y) == 0x41) { // Push up if too low
		e->y -= block_to_sub(1);
	}
}

void ai_door_onUpdate(Entity *e) {
	if(e->direction) {
		SPR_SAFERELEASE(e->sprite);
	} else if(e->sprite == NULL) {
		entity_sprite_create(e);
	}
}

void ai_theDoor_onUpdate(Entity *e) {
	if(e->state == 1) {
		e->state_time--;
		if(e->state_time == 0) e->state = 0;
	} else {
		if(e->damage_time) {
			e->state = 1;
			e->state_time = TIME(30);
			SPR_SAFEFRAME(e->sprite, 3);
		} else {
			s32 x1 = e->x - block_to_sub(6), y1 = e->y - block_to_sub(6),
				x2 = e->x + block_to_sub(6), y2 = e->y + block_to_sub(6);
			if(player.x > x1 && player.x < x2 && player.y > y1 && player.y < y2) {
				SPR_SAFEFRAME(e->sprite, 2);
			} else {
				SPR_SAFEFRAME(e->sprite, 0);
			}
		}
	}
}

void oncreate_doorway(Entity *e) {
	if(e->eflags & NPC_OPTION2) {
		// Right door
		e->spriteAnim = 1;
		e->x -= 8 << CSF;
	} else {
		// Left door
		e->x += 8 << CSF;
	}
}

void ai_doorway(Entity *e) {

}
