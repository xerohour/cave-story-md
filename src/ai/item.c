#include "ai.h"

#include <genesis.h>
#include "audio.h"
#include "player.h"
#include "stage.h"
#include "tables.h"
#include "tsc.h"
#include "input.h"
#include "system.h"
#include "resources.h"
#include "effect.h"

void ai_energy_onCreate(Entity *e) {
	if(!(e->eflags & NPC_OPTION2)) {
		e->display_box.left -= 4;
		e->display_box.right -= 4;
	}
	e->x_speed = 0x200 - (random() % 0x400);
	e->alwaysActive = true;
}

void ai_energy_onUpdate(Entity *e) {
	if(entity_overlapping(&player, e)) {
		Weapon *w = &playerWeapon[currentWeapon];
		if(w->level == 3 && w->energy + e->experience > 
				weapon_info[w->type].experience[w->level-1]) {
			w->energy = weapon_info[w->type].experience[w->level-1];
		} else {
			w->energy += e->experience;
		}
		if(w->level < 3 && w->energy >= weapon_info[w->type].experience[w->level-1]) {
			sound_play(SND_LEVEL_UP, 5);
			w->energy -= weapon_info[w->type].experience[w->level-1];
			w->level++;
		} else {
			sound_play(SND_GET_XP, 5);
			player.damage_time = 30;
			player.damage_value += e->experience;
		}
		e->state = STATE_DELETE;
	} else {
		e->state_time++;
		if(e->state_time > 10 * FPS) {
			e->state = STATE_DELETE;
			return;
		} else if(e->state_time > 7 * FPS) {
			SPR_SAFEVISIBILITY(e->sprite, (e->state_time & 3) > 1 ? AUTO_FAST : HIDDEN);
		} else if(!entity_on_screen(e)) {
			SPR_SAFERELEASE(e->sprite);
		} else if(e->sprite == NULL) {
			entity_sprite_create(e);
		}
		e->y_speed += 0x10;
		if(e->y_speed > 0x400) e->y_speed = 0x400;
		// Check below / above first
		u8 block_below = stage_get_block_type(
				sub_to_block(e->x), sub_to_block(e->y + 0x800));
		u8 block_above = stage_get_block_type(
				sub_to_block(e->x), sub_to_block(e->y - 0x800));
		if(block_below == 0x41 || block_below == 0x43) {
			e->y -= sub_to_pixel(e->y + 0x800) % 16;
			e->y_speed = -e->y_speed >> 1;
			if(e->y_speed > -0x400) e->y_speed = -0x400;
			sound_play(SND_XP_BOUNCE, 0);
		} else if(block_below & BLOCK_SLOPE) {
			u8 index = block_below & 0xF;
			if(index >= 4) {
				u16 xx = sub_to_pixel(e->x);
				u16 yy = sub_to_pixel(e->y + 0x800);
				s8 overlap = (yy % 16) - heightmap[index % 4][xx % 16];
				if(overlap >= 0) {
					e->y -= overlap;
					if(e->y_speed >= 0x200) sound_play(SND_XP_BOUNCE, 0);
					e->y_speed = -e->y_speed;
					if(e->y_speed > -0x400) e->y_speed = -0x400;
				}
			}
		} else if(block_above == 0x41 || block_above == 0x43) {
			e->y_speed = -e->y_speed >> 1;
			if(e->y_speed < 0x300) e->y_speed = 0x300;
		} else {
			e->y_speed += GRAVITY;
			if(e->y_speed > 0x600) e->y_speed = 0x600;
		}
		// Check in front
		u8 block_front = stage_get_block_type(
				sub_to_block(e->x + (e->x_speed > 0 ? 0x800 : -0x800)),
				sub_to_block(e->y - 0x100));
		if(block_front == 0x41 || block_front == 0x43) { // hit a wall
			e->x_speed = -e->x_speed;
		}
		e->x += e->x_speed;
		e->y += e->y_speed;
	}
}

void ai_missile_onUpdate(Entity *e) {
	// Dropped health/missiles should be deleted after 10 seconds, even if it is offscreen
	if(!e->state) {
		e->alwaysActive = (e->eflags & NPC_OPTION1) > 0;
		e->state = 1;
	}
	// Hide the sprite when under a breakable block
	// Reduces unnecessary lag in sand zone
	if(stage_get_block_type(sub_to_block(e->x), sub_to_block(e->y)) == 0x43) {
		SPR_SAFERELEASE(e->sprite);
		return;
	} else if(e->sprite == NULL) {
		entity_sprite_create(e);
	}
	// Increases missile ammo, plays sound and deletes itself
	if(entity_overlapping(&player, e)) {
		// Find missile or super missile
		Weapon *w = player_find_weapon(WEAPON_MISSILE);
		if(w == NULL) w = player_find_weapon(WEAPON_SUPERMISSILE);
		// If we found either increase ammo
		if(w != NULL) {
			// OPTION2 is large pickup
			w->ammo += (e->eflags & NPC_OPTION2) ? 3 : 1;
			if(w->ammo >= w->maxammo) w->ammo = w->maxammo;
		}
		sound_play(SND_GET_MISSILE, 5);
		e->state = STATE_DELETE;
	} else if(e->eflags & NPC_OPTION1) {
		e->state_time++;
		if(e->state_time > 10 * FPS) {
			e->state = STATE_DELETE;
			return;
		} else if(e->state_time > 7 * FPS) {
			SPR_SAFEVISIBILITY(e->sprite, (e->state_time & 3) > 1 ? AUTO_FAST : HIDDEN);
		}
	}
}

void ai_heart_onUpdate(Entity *e) {
	if(!e->state) {
		e->alwaysActive = (e->eflags & NPC_OPTION1) > 0;
		e->state = 1;
	}
	// Hide the sprite when under a breakable block
	// Reduces unnecessary lag in sand zone
	if(stage_get_block_type(sub_to_block(e->x), sub_to_block(e->y)) == 0x43) {
		SPR_SAFERELEASE(e->sprite);
		return;
	} else if(e->sprite == NULL) {
		entity_sprite_create(e);
	}
	// Increases health, plays sound and deletes itself
	if(entity_overlapping(&player, e)) {
		if(e->eflags & NPC_OPTION1) {
			player.health += e->health;
		} else if(e->eflags & NPC_OPTION2) {
			player.health += 5;
		} else {
			player.health += 2;
		}
		// Don't go over max health
		if(player.health >= playerMaxHealth) player.health = playerMaxHealth;
		sound_play(SND_HEALTH_REFILL, 5);
		e->state = STATE_DELETE;
	} else if(e->eflags & NPC_OPTION1) {
		e->state_time++;
		if(e->state_time > 10 * FPS) {
			e->state = STATE_DELETE;
			return;
		} else if(e->state_time > 7 * FPS) {
			SPR_SAFEVISIBILITY(e->sprite, (e->state_time & 3) > 1 ? AUTO_FAST : HIDDEN);
		}
	}
}

void ai_hiddenPowerup_onCreate(Entity *e) {
	e->eflags |= NPC_SHOOTABLE;
}

void ai_hiddenPowerup_onUpdate(Entity *e) {
	if(e->health < 990) {
		effect_create_smoke(0, sub_to_pixel(e->x), sub_to_pixel(e->y));
		sound_play(SND_EXPL_SMALL, 5);
		if(e->eflags & NPC_OPTION2) {
			e->type = OBJ_MISSILE;
			SPR_SAFEADD(e->sprite, &SPR_MisslP, sub_to_pixel(e->x), sub_to_pixel(e->y),
				TILE_ATTR(PAL1, 0, 0, 0), 4);
			e->eflags &= ~NPC_OPTION2;
		} else {
			e->type = OBJ_HEART;
			SPR_SAFEADD(e->sprite, &SPR_Heart, sub_to_pixel(e->x), sub_to_pixel(e->y),
				TILE_ATTR(PAL1, 0, 0, 0), 4);
			e->health = 2;
		}
		e->eflags &= ~NPC_SHOOTABLE;
		e->nflags &= ~NPC_SHOOTABLE;
	}
}
