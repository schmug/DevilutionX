#pragma once

#include "monster.h"
#include "monstdat.h"

namespace devilution {

/**
 * @brief Maps new monster types to existing monster sprites
 * This allows us to reuse existing sprite assets for our new monsters
 * @param type The monster type to get the sprite mapping for
 * @return The monster type to use for sprite loading
 */
inline _monster_id GetMonsterSpriteMapping(_monster_id type)
{
	// Default to the original type if no mapping exists
	_monster_id spriteType = type;

	// Map new monster types to existing sprites
	switch (type) {
		// Fallen family
		case MT_FALLENSHAMAN:
			spriteType = MT_NGOATMC; // Use Goat Man sprites for Fallen Shaman
			break;
		case MT_CARVER:
		case MT_DEVILKIN:
		case MT_DARKONE:
			spriteType = MT_RFALLSP; // Use Fallen sprites for Carver, Devilkin, and Dark One
			break;

		// Rodent family
		case MT_QUILLRAT:
		case MT_SPIKERAT:
			spriteType = MT_RSKELBW; // Use Skeleton with Bow sprites for Quill/Spike Rats
			break;

		// Fetish family
		case MT_FETISHDART:
			spriteType = MT_RFALLSD; // Use Fallen with Spear sprites for Fetish Dart
			break;
		case MT_FETISHSHAMAN:
			spriteType = MT_NGOATMC; // Use Goat Man sprites for Fetish Shaman
			break;

		// Brute and Vile family
		case MT_BRUTE:
			spriteType = MT_SNEAK; // Use Sneak sprites for Brute
			break;
		case MT_VILEMOTHER:
			spriteType = MT_NACID; // Use Acid Beast sprites for Vile Mother
			break;
		case MT_VILEDOG:
			spriteType = MT_NSCAV; // Use Scavenger sprites for Vile Dog
			break;

		// Undead family
		case MT_UNDEADARCHER:
			spriteType = MT_RSKELBW; // Use Skeleton with Bow sprites for Undead Archer
			break;
		case MT_DOOMKNIGHT:
			spriteType = MT_WSKELAX; // Use Skeleton with Axe sprites for Doom Knight
			break;
		case MT_GHOUL:
			spriteType = MT_NZOMBIE; // Use Zombie sprites for Ghoul
			break;
		case MT_WRAITH:
		case MT_SPECTER:
			spriteType = MT_NSCAV; // Use Scavenger sprites for Wraith and Specter
			break;

		// Poison family
		case MT_POISONSPITTER:
			spriteType = MT_NACID; // Use Acid Beast sprites for Poison Spitter
			break;

		// Plant family
		case MT_TREEENT:
			spriteType = MT_FIEND; // Use Fiend sprites for Treeent
			break;

		// Boss monsters
		case MT_ANDARIEL:
			spriteType = MT_UNSEEN; // Use Unseen sprites for Andariel
			break;
		case MT_BLOODRAVEN:
			spriteType = MT_RSKELBW; // Use Skeleton with Bow sprites for Blood Raven
			break;
		case MT_SUMMONER:
			spriteType = MT_ILLWEAV; // Use Illusion Weaver sprites for The Summoner
			break;
		case MT_DURIEL:
			spriteType = MT_FIEND; // Use Fiend sprites for Duriel
			break;
		case MT_MEPHISTO:
			spriteType = MT_LRDSAYTR; // Use Lord Sayter sprites for Mephisto
			break;
		case MT_IZUAL:
			spriteType = MT_FAMILIAR; // Use Familiar sprites for Izual
			break;
		case MT_BAAL:
			spriteType = MT_DIABLO; // Use Diablo sprites for Baal
			break;
		case MT_GRISWOLD:
			spriteType = MT_NGOATMC; // Use Goat Man sprites for Griswold
			break;

		default:
			// Use the original type if no mapping exists
			break;
	}

	return spriteType;
}

} // namespace devilution
