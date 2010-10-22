/*
 * Copyright 2010 Daniel Albano
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "LanguageState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "MainMenuState.h"

/**
 * Initializes all the elements in the Language window.
 * @param game Pointer to the core game.
 */
LanguageState::LanguageState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnEnglish = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_btnGerman = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 20, 64, 118);
	_btnFrench = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 20, 64, 146);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	
	add(_window);
	add(_btnEnglish);
	add(_btnGerman);
	add(_btnFrench);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));
	
	_btnEnglish->setColor(Palette::blockOffset(8)+8);
	_btnEnglish->setText("ENGLISH");
	_btnEnglish->onMouseClick((EventHandler)&LanguageState::btnEnglishClick);
	
	_btnGerman->setColor(Palette::blockOffset(8)+8);
	_btnGerman->setText("DEUTSCHE");
	_btnGerman->onMouseClick((EventHandler)&LanguageState::btnGermanClick);

	_btnFrench->setColor(Palette::blockOffset(8)+8);
	_btnFrench->setText("FRANCAIS");
	_btnFrench->onMouseClick((EventHandler)&LanguageState::btnFrenchClick);
}

/**
 *
 */
LanguageState::~LanguageState()
{
	
}

void LanguageState::changeLanguage(string lang)
{
	_game->getResourcePack()->setLanguage(lang);
	_game->setState(new MainMenuState(_game));
}

/**
 * Sets the language to English and opens
 * the Main Menu window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void LanguageState::btnEnglishClick(SDL_Event *ev, int scale)
{
	changeLanguage("ENGLISH.DAT");
}

/**
 * Sets the language to German and opens
 * the Main Menu window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void LanguageState::btnGermanClick(SDL_Event *ev, int scale)
{
	changeLanguage("GERMAN.DAT");
}

/**
 * Sets the language to French and opens
 * the Main Menu window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void LanguageState::btnFrenchClick(SDL_Event *ev, int scale)
{
	changeLanguage("FRENCH.DAT");
}