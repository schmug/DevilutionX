/**
 * @file gamemenu.h
 *
 * Interface of the in-game menu functions.
 */
#pragma once

namespace devilution {


void gamemenu_on();
void gamemenu_off();
void gamemenu_handle_previous();
void gamemenu_exit_game(bool bActivate);
void gamemenu_quit_game(bool bActivate);
void gamemenu_load_game(bool bActivate);
void gamemenu_save_game(bool bActivate);

// Drop rate menu functions
void GamemenuDropRateModifiers(bool bActivate);
void GamemenuGoldDropRate(bool bActivate);
void GamemenuGoldAmount(bool bActivate);
void GamemenuResetDropRates(bool bActivate);

// Legacy drop rate adjustment functions (to be deprecated)
void HandleDropRateKeyPress(int key);
bool IsInDropRateAdjustmentMode();
void ExitDropRateAdjustmentMode();

extern bool isGameMenuOpen;
extern bool inDropRateAdjustmentMode;

} // namespace devilution
