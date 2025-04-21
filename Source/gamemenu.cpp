/**
 * @file gamemenu.cpp
 *
 * Implementation of the in-game menu functions.
 */
#include "gamemenu.h"

#include "cursor.h"
#include "diablo_msg.hpp"
#include "engine/backbuffer_state.hpp"
#include "engine/demomode.h"
#include "engine/events.hpp"
#include "engine/sound.h"
#include "engine/sound_defs.hpp"
#include "gmenu.h"
#include "headless_mode.hpp"
#include "loadsave.h"
#include "multi.h"
#include "options.h"
#include "pfile.h"
#include "qol/floatingnumbers.h"
#include "utils/language.h"
#include "utils/file_util.h"
#include "utils/paths.h"
#include "mods/config/drop_rate_config.h"
#include "mods/drop_rate_modifier.h"
#include "items.h"
#include <fmt/format.h>

#ifndef USE_SDL1
#include "controls/touch/renderers.h"
#endif

namespace devilution {

bool isGameMenuOpen = false;
bool inDropRateAdjustmentMode = false;

namespace {

// Forward-declare menu handlers, used by the global menu structs below.
void GamemenuPrevious(bool bActivate);
void GamemenuNewGame(bool bActivate);
void GamemenuOptions(bool bActivate);
void GamemenuMusicVolume(bool bActivate);
void GamemenuSoundVolume(bool bActivate);
void GamemenuBrightness(bool bActivate);
void GamemenuSpeed(bool bActivate);
void GamemenuDropRateModifiers(bool bActivate);
void GamemenuGoldDropRate(bool bActivate);
void GamemenuGoldAmount(bool bActivate);
void GamemenuResetDropRates(bool bActivate);
void GamemenuGetGoldDropRate();
void GamemenuGetGoldAmount();
int GamemenuSliderGoldDropRate();
int GamemenuSliderGoldAmount();

/** Contains the game menu items of the single player menu. */
TMenuItem sgSingleMenu[] = {
	// clang-format off
	// dwFlags,      pszStr,                  fnMenu
	{ GMENU_ENABLED, N_("Options"),           &GamemenuOptions    },
	{ GMENU_ENABLED, N_("Drop Rate Mods"),    &GamemenuDropRateModifiers },
	{ GMENU_ENABLED, N_("Save Game"),         &gamemenu_save_game },
	{ GMENU_ENABLED, N_("Load Game"),         &gamemenu_load_game },
	{ GMENU_ENABLED, N_("Exit to Main Menu"), &GamemenuNewGame    },
	{ GMENU_ENABLED, N_("Quit Game"),         &gamemenu_quit_game },
	{ GMENU_ENABLED, nullptr,                 nullptr             },
	// clang-format on
};
/** Contains the game menu items of the multi player menu. */
TMenuItem sgMultiMenu[] = {
	// clang-format off
	// dwFlags,      pszStr,                  fnMenu
	{ GMENU_ENABLED, N_("Options"),           &GamemenuOptions    },
	{ GMENU_ENABLED, N_("Drop Rate Mods"),    &GamemenuDropRateModifiers },
	{ GMENU_ENABLED, N_("Exit to Main Menu"), &GamemenuNewGame    },
	{ GMENU_ENABLED, N_("Quit Game"),         &gamemenu_quit_game },
	{ GMENU_ENABLED, nullptr,                 nullptr             },
	// clang-format on
};
TMenuItem sgOptionsMenu[] = {
	// clang-format off
	// dwFlags,                     pszStr,              fnMenu
	{ GMENU_ENABLED | GMENU_SLIDER, nullptr,             &GamemenuMusicVolume  },
	{ GMENU_ENABLED | GMENU_SLIDER, nullptr,             &GamemenuSoundVolume  },
	{ GMENU_ENABLED | GMENU_SLIDER, N_("Gamma"),         &GamemenuBrightness   },
	{ GMENU_ENABLED | GMENU_SLIDER, N_("Speed"),         &GamemenuSpeed        },
	{ GMENU_ENABLED               , N_("Previous Menu"), &GamemenuPrevious     },
	{ GMENU_ENABLED               , nullptr,             nullptr               },
	// clang-format on
};
/** Specifies the menu names for music enabled and disabled. */
const char *const MusicToggleNames[] = {
	N_("Music"),
	N_("Music Disabled"),
};
/** Specifies the menu names for sound enabled and disabled. */
const char *const SoundToggleNames[] = {
	N_("Sound"),
	N_("Sound Disabled"),
};

void GamemenuUpdateSingle()
{
	sgSingleMenu[2].setEnabled(gbValidSaveFile);

	bool enable = MyPlayer->_pmode != PM_DEATH && !MyPlayerIsDead;

	sgSingleMenu[0].setEnabled(enable);
}

void GamemenuPrevious(bool /*bActivate*/)
{
	gamemenu_on();
}

void GamemenuNewGame(bool /*bActivate*/)
{
	for (Player &player : Players) {
		player._pmode = PM_QUIT;
		player._pInvincible = true;
	}

	MyPlayerIsDead = false;
	if (!HeadlessMode) {
		RedrawEverything();
		scrollrt_draw_game_screen();
	}
	CornerStone.activated = false;
	gbRunGame = false;
	gamemenu_off();
}

void GamemenuSoundMusicToggle(const char *const *names, TMenuItem *menuItem, int volume)
{
	if (gbSndInited) {
		menuItem->addFlags(GMENU_ENABLED | GMENU_SLIDER);
		menuItem->pszStr = names[0];
		gmenu_slider_steps(menuItem, VOLUME_STEPS);
		gmenu_slider_set(menuItem, VOLUME_MIN, VOLUME_MAX, volume);
		return;
	}

	menuItem->removeFlags(GMENU_ENABLED | GMENU_SLIDER);
	menuItem->pszStr = names[1];
}

int GamemenuSliderMusicSound(TMenuItem *menuItem)
{
	return gmenu_slider_get(menuItem, VOLUME_MIN, VOLUME_MAX);
}

void GamemenuGetMusic()
{
	GamemenuSoundMusicToggle(MusicToggleNames, sgOptionsMenu, sound_get_or_set_music_volume(1));
}

void GamemenuGetSound()
{
	GamemenuSoundMusicToggle(SoundToggleNames, &sgOptionsMenu[1], sound_get_or_set_sound_volume(1));
}

void GamemenuGetBrightness()
{
	gmenu_slider_steps(&sgOptionsMenu[2], 21);
	gmenu_slider_set(&sgOptionsMenu[2], 0, 100, UpdateBrightness(-1));
}

void GamemenuGetSpeed()
{
	if (gbIsMultiplayer) {
		sgOptionsMenu[3].removeFlags(GMENU_ENABLED | GMENU_SLIDER);
		if (sgGameInitInfo.nTickRate >= 50)
			sgOptionsMenu[3].pszStr = _("Speed: Fastest").data();
		else if (sgGameInitInfo.nTickRate >= 40)
			sgOptionsMenu[3].pszStr = _("Speed: Faster").data();
		else if (sgGameInitInfo.nTickRate >= 30)
			sgOptionsMenu[3].pszStr = _("Speed: Fast").data();
		else if (sgGameInitInfo.nTickRate == 20)
			sgOptionsMenu[3].pszStr = _("Speed: Normal").data();
		return;
	}

	sgOptionsMenu[3].addFlags(GMENU_ENABLED | GMENU_SLIDER);

	sgOptionsMenu[3].pszStr = _("Speed").data();
	gmenu_slider_steps(&sgOptionsMenu[3], 46);
	gmenu_slider_set(&sgOptionsMenu[3], 20, 50, sgGameInitInfo.nTickRate);
}

// Drop rate menu items
TMenuItem sgDropRateMenu[] = {
	// clang-format off
	// dwFlags,                     pszStr,                  fnMenu
	{ GMENU_ENABLED | GMENU_SLIDER, N_("Gold Drop Rate"),    &GamemenuGoldDropRate },
	{ GMENU_ENABLED | GMENU_SLIDER, N_("Gold Amount"),       &GamemenuGoldAmount   },
	{ GMENU_ENABLED               , N_("Reset to Default"), &GamemenuResetDropRates },
	{ GMENU_ENABLED               , N_("Previous Menu"),    &GamemenuPrevious     },
	{ GMENU_ENABLED               , nullptr,                nullptr               },
	// clang-format on
};

int GamemenuSliderBrightness()
{
	return gmenu_slider_get(&sgOptionsMenu[2], 0, 100);
}

int GamemenuSliderGoldDropRate()
{
	return gmenu_slider_get(&sgDropRateMenu[0], 0, 100);
}

int GamemenuSliderGoldAmount()
{
	return gmenu_slider_get(&sgDropRateMenu[1], 0, 100);
}

void GamemenuOptions(bool /*bActivate*/)
{
	GamemenuGetMusic();
	GamemenuGetSound();
	GamemenuGetBrightness();
	GamemenuGetSpeed();
	gmenu_set_items(sgOptionsMenu, nullptr);
}

void GamemenuMusicVolume(bool bActivate)
{
	if (bActivate) {
		if (gbMusicOn) {
			gbMusicOn = false;
			music_stop();
			sound_get_or_set_music_volume(VOLUME_MIN);
		} else {
			gbMusicOn = true;
			sound_get_or_set_music_volume(VOLUME_MAX);
			music_start(GetLevelMusic(leveltype));
		}
	} else {
		int volume = GamemenuSliderMusicSound(&sgOptionsMenu[0]);
		sound_get_or_set_music_volume(volume);
		if (volume == VOLUME_MIN) {
			if (gbMusicOn) {
				gbMusicOn = false;
				music_stop();
			}
		} else if (!gbMusicOn) {
			gbMusicOn = true;
			music_start(GetLevelMusic(leveltype));
		}
	}

	GamemenuGetMusic();
}

void GamemenuSoundVolume(bool bActivate)
{
	if (bActivate) {
		if (gbSoundOn) {
			gbSoundOn = false;
			sound_stop();
			sound_get_or_set_sound_volume(VOLUME_MIN);
		} else {
			gbSoundOn = true;
			sound_get_or_set_sound_volume(VOLUME_MAX);
		}
	} else {
		int volume = GamemenuSliderMusicSound(&sgOptionsMenu[1]);
		sound_get_or_set_sound_volume(volume);
		if (volume == VOLUME_MIN) {
			if (gbSoundOn) {
				gbSoundOn = false;
				sound_stop();
			}
		} else if (!gbSoundOn) {
			gbSoundOn = true;
		}
	}
	PlaySFX(SfxID::MenuMove);
	GamemenuGetSound();
}

void GamemenuBrightness(bool bActivate)
{
	int brightness;
	if (bActivate) {
		brightness = UpdateBrightness(-1);
		brightness = (brightness == 0) ? 100 : 0;
	} else {
		brightness = GamemenuSliderBrightness();
	}

	UpdateBrightness(brightness);
	GamemenuGetBrightness();
}

void GamemenuSpeed(bool bActivate)
{
	if (bActivate) {
		if (sgGameInitInfo.nTickRate != 20)
			sgGameInitInfo.nTickRate = 20;
		else
			sgGameInitInfo.nTickRate = 50;
		gmenu_slider_set(&sgOptionsMenu[3], 20, 50, sgGameInitInfo.nTickRate);
	} else {
		sgGameInitInfo.nTickRate = gmenu_slider_get(&sgOptionsMenu[3], 20, 50);
	}

	GetOptions().Gameplay.tickRate.SetValue(sgGameInitInfo.nTickRate);
	gnTickDelay = 1000 / sgGameInitInfo.nTickRate;
}

// The drop rate menu items are already declared at the top of the file

// Function to get the current gold drop rate
void GamemenuGetGoldDropRate()
{
	auto& manager = DropRateManager::getInstance();
	int currentRate = manager.GetGoldDropRatePercent();
	
	// Set the slider to the current value
	gmenu_slider_steps(&sgDropRateMenu[0], 100);
	gmenu_slider_set(&sgDropRateMenu[0], 0, 100, currentRate);
	
	// Log the current rate for debugging
	LogInfo("GamemenuGetGoldDropRate: Current gold drop rate is {}%", currentRate);
}

// Function to get the current gold amount
void GamemenuGetGoldAmount()
{
	auto& manager = DropRateManager::getInstance();
	int currentAmount = manager.GetGoldAmountPercent();
	
	// Set the slider to the current value
	gmenu_slider_steps(&sgDropRateMenu[1], 100);
	gmenu_slider_set(&sgDropRateMenu[1], 0, 100, currentAmount);
	
	// Log the current amount for debugging
	LogInfo("GamemenuGetGoldAmount: Current gold amount is {}%", currentAmount);
}

// Handler for the Drop Rate Modifiers menu
void GamemenuDropRateModifiers(bool bActivate)
{
	if (!bActivate)
		return;
	
	// Initialize the drop rate menu
	GamemenuGetGoldDropRate();
	GamemenuGetGoldAmount();
	
	// Display the menu
	gmenu_set_items(sgDropRateMenu, nullptr);
}

// Handler for the Gold Drop Rate slider
void GamemenuGoldDropRate(bool bActivate)
{
	auto& manager = DropRateManager::getInstance();
	
	if (bActivate) {
		// Toggle between 0% and 100% when activated
		int currentRate = manager.GetGoldDropRatePercent();
		int newRate = (currentRate > 0) ? 0 : 100;
		manager.SetGoldDropRatePercent(newRate);
	} else {
		// Get the slider value and update the drop rate
		int rate = GamemenuSliderGoldDropRate();
		manager.SetGoldDropRatePercent(rate);
	}
	
	// Update the slider
	GamemenuGetGoldDropRate();
	
	// Play a sound effect for feedback
	PlaySFX(SfxID::MenuMove);
}

// Handler for the Gold Amount slider
void GamemenuGoldAmount(bool bActivate)
{
	auto& manager = DropRateManager::getInstance();
	
	if (bActivate) {
		// Toggle between 0% and 100% when activated
		int currentAmount = manager.GetGoldAmountPercent();
		int newAmount = (currentAmount > 0) ? 0 : 100;
		manager.SetGoldAmountPercent(newAmount);
	} else {
		// Get the slider value and update the gold amount
		int amount = GamemenuSliderGoldAmount();
		manager.SetGoldAmountPercent(amount);
		
		// Log the change
		LogInfo("GamemenuGoldAmount: Updated gold amount to {}%", amount);
	}
	
	// Update the slider
	GamemenuGetGoldAmount();
	
	// Play a sound effect for feedback
	PlaySFX(SfxID::MenuMove);
}

// Handler for the Reset to Default button
void GamemenuResetDropRates(bool bActivate)
{
	if (!bActivate)
		return;
	
	// Reset the drop rates to default values
	auto& manager = DropRateManager::getInstance();
	manager.ResetDropRatesToDefaults();
	
	// Update the sliders
	GamemenuGetGoldDropRate();
	GamemenuGetGoldAmount();
	
	// Log the reset
	LogInfo("GamemenuResetDropRates: Reset drop rates to defaults");
	
	// Play a sound effect for feedback
	PlaySFX(SfxID::MenuMove);
	
	// Show a brief message to confirm the reset
	InitDiabloMsg("Drop rates reset to defaults");
}

} // namespace

void gamemenu_exit_game(bool bActivate)
{
	GamemenuNewGame(bActivate);
}

void gamemenu_quit_game(bool bActivate)
{
	GamemenuNewGame(bActivate);
#ifndef NOEXIT
	gbRunGameResult = false;
#else
	ReturnToMainMenu = true;
#endif
}

void gamemenu_load_game(bool /*bActivate*/)
{
	EventHandler saveProc = SetEventHandler(DisableInputEventHandler);
	gamemenu_off();
	ClearFloatingNumbers();
	NewCursor(CURSOR_NONE);
	InitDiabloMsg(EMSG_LOADING);
	RedrawEverything();
	DrawAndBlit();
#ifndef USE_SDL1
	DeactivateVirtualGamepad();
	FreeVirtualGamepadTextures();
#endif
	if (tl::expected<void, std::string> result = LoadGame(false); !result.has_value()) {
		app_fatal(result.error());
	}
#if !defined(USE_SDL1) && !defined(__vita__)
	if (renderer != nullptr) {
		InitVirtualGamepadTextures(*renderer);
	}
#endif
	NewCursor(CURSOR_HAND);
	ClrDiabloMsg();
	CornerStone.activated = false;
	PaletteFadeOut(8);
	MyPlayerIsDead = false;
	RedrawEverything();
	DrawAndBlit();
	LoadPWaterPalette();
	PaletteFadeIn(8);
	NewCursor(CURSOR_HAND);
	interface_msg_pump();
	SetEventHandler(saveProc);
}

void gamemenu_save_game(bool /*bActivate*/)
{
	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (MyPlayer->_pmode == PM_DEATH || MyPlayerIsDead) {
		gamemenu_off();
		return;
	}

	EventHandler saveProc = SetEventHandler(DisableInputEventHandler);
	NewCursor(CURSOR_NONE);
	gamemenu_off();
	InitDiabloMsg(EMSG_SAVING);
	RedrawEverything();
	DrawAndBlit();
	uint32_t currentTime = SDL_GetTicks();
	SaveGame();
	ClrDiabloMsg();
	InitDiabloMsg(EMSG_GAME_SAVED, currentTime + 1000 - SDL_GetTicks());
	RedrawEverything();
	NewCursor(CURSOR_HAND);
	if (CornerStone.activated) {
		CornerstoneSave();
		if (!demo::IsRunning()) SaveOptions();
	}
	interface_msg_pump();
	SetEventHandler(saveProc);
}

void gamemenu_on()
{
	isGameMenuOpen = true;
	if (!gbIsMultiplayer) {
		gmenu_set_items(sgSingleMenu, GamemenuUpdateSingle);
	} else {
		gmenu_set_items(sgMultiMenu, nullptr);
	}
	PressEscKey();
}

void gamemenu_off()
{
	isGameMenuOpen = false;
	gmenu_set_items(nullptr, nullptr);
}

void gamemenu_handle_previous()
{
	if (gmenu_is_active())
		gamemenu_off();
	else
		gamemenu_on();
}

// Function to check if we're in drop rate adjustment mode
bool IsInDropRateAdjustmentMode()
{
	return inDropRateAdjustmentMode;
}

// Function to exit drop rate adjustment mode
void ExitDropRateAdjustmentMode()
{
	inDropRateAdjustmentMode = false;
}

// Handler for key presses in drop rate adjustment mode
// Note: This function is now deprecated as we're using the slider-based menu
// It's kept for backward compatibility but should be removed in the future
void HandleDropRateKeyPress(int key)
{
	LogInfo("HandleDropRateKeyPress: Received key {}", key);
	
	// This function is deprecated - we're now using the slider-based menu
	// Just exit drop rate adjustment mode and open the new menu
	inDropRateAdjustmentMode = false;
	
	// Show a message to inform the user about the new menu
	InitDiabloMsg("Drop rate adjustment is now available in the game menu");
	
	// Open the game menu with the drop rate options
	gamemenu_on();
	// We need to call the function differently to avoid ambiguity
	// Let's just show the message and let the user navigate to the menu
	InitDiabloMsg("Use the Drop Rate Mods option in the game menu");
}

} // namespace devilution
