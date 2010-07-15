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
#include "BaseView.h"
#include <sstream>
#include <cmath>
#include "SurfaceSet.h"
#include "Base.h"
#include "BaseFacility.h"
#include "RuleBaseFacility.h"
#include "Craft.h"
#include "RuleCraft.h"
#include "Text.h"
#include "Palette.h"
#include "Timer.h"

using namespace std;

#define GRID_SIZE 32

/**
 * Sets up a base view with the specified size and position.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
BaseView::BaseView(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _base(0), _texture(0), _selFacility(0), _big(big), _small(small), _gridX(0), _gridY(0), _selSize(0), _selector(0), _blink(true)
{
	for (int x = 0; x < BASE_SIZE; x++)
		for (int y = 0; y < BASE_SIZE; y++)
			_facilities[x][y] = 0;

	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&BaseView::blink);
	_timer->start();
}

/**
 * Deletes the child selector.
 */
BaseView::~BaseView()
{
	if (_selector != 0)
		delete _selector;
}

/**
 * Changes the current base to display and
 * initalizes the internal base grid.
 * @param base Pointer to base to display.
 */
void BaseView::setBase(Base *base)
{
	_base = base;
	_selFacility = 0;

	// Clear grid
	for (int x = 0; x < BASE_SIZE; x++)
		for (int y = 0; y < BASE_SIZE; y++)
			_facilities[x][y] = 0;

	// Fill grid with base facilities
	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{
				_facilities[x][y] = *i;
			}
		}
	}

	draw();
}

/**
 * Changes the texture to use for drawing
 * the various base elements.
 * @param texture Pointer to SurfaceSet to use.
 */
void BaseView::setTexture(SurfaceSet *texture)
{
	_texture = texture;
}

/**
 * Returns the facility the mouse is currently over.
 * @return Pointer to base facility (NULL if none).
 */
BaseFacility *BaseView::getSelectedFacility()
{
	return _selFacility;
}

/**
 * Returns the X position of the grid square 
 * the mouse is currently over.
 * @return X position on the grid.
 */
int BaseView::getGridX()
{
	return _gridX;
}

/**
 * Returns the Y position of the grid square 
 * the mouse is currently over.
 * @return Y position on the grid.
 */
int BaseView::getGridY()
{
	return _gridY;
}

/**
 * If enabled, the base view will respond to player input,
 * highlighting the selected facility.
 * @param size Facility length (0 disables it).
 */
void BaseView::setSelectable(int size)
{
	_selSize = size;
	if (_selSize > 0)
	{
		_selector = new Surface(size * GRID_SIZE, size * GRID_SIZE, _x, _y);
		_selector->setPalette(getPalette());
		SDL_Rect r;
		r.w = _selector->getWidth();
		r.h = _selector->getHeight();
		r.x = 0;
		r.y = 0;
		SDL_FillRect(_selector->getSurface(), &r, Palette::blockOffset(1));
		r.w -= 2;
		r.h -= 2;
		r.x++;
		r.y++;
		SDL_FillRect(_selector->getSurface(), &r, 0);
		_selector->setVisible(false);
	}
	else
	{
		delete _selector;
	}
}

/**
 * Returns if a certain facility can be successfully
 * placed on the currently selected square.
 * @param rule Facility type.
 * @return True if placeable, False otherwise.
 */
bool BaseView::isPlaceable(RuleBaseFacility *rule)
{
	// Check if square isn't occupied
	for (int y = _gridY; y < _gridY + rule->getSize(); y++)
	{
		for (int x = _gridX; x < _gridX + rule->getSize(); x++)
		{
			if (x > BASE_SIZE || y > BASE_SIZE)
				return false;
			if (_facilities[x][y] != 0)
				return false;
		}
	}

	// Check for another facility to connect to
	for (int i = 0; i < rule->getSize(); i++)
	{
		if ((_gridX > 0 && _facilities[_gridX - 1][_gridY + i] != 0 && _facilities[_gridX - 1][_gridY + i]->getBuildTime() == 0) ||
			(_gridY > 0 && _facilities[_gridX + i][_gridY - 1] != 0 && _facilities[_gridX + i][_gridY - 1]->getBuildTime() == 0) ||
			(_gridX + rule->getSize() < BASE_SIZE && _facilities[_gridX + rule->getSize()][_gridY + i] != 0 && _facilities[_gridX + rule->getSize()][_gridY + i]->getBuildTime() == 0) ||
			(_gridY + rule->getSize() < BASE_SIZE && _facilities[_gridX + i][_gridY + rule->getSize()] != 0 && _facilities[_gridX + i][_gridY + rule->getSize()]->getBuildTime() == 0))
			return true;
	}

	return false;
}

/**
 * Keeps the animation timers running.
 */
void BaseView::think()
{
	_timer->think(0, this);
}

/**
 * Makes the facility selector blink.
 */
void BaseView::blink()
{
	_blink = !_blink;

	if (_selSize > 0)
	{
		SDL_Rect r;
		if (_blink)
		{
			r.w = _selector->getWidth();
			r.h = _selector->getHeight();
			r.x = 0;
			r.y = 0;
			SDL_FillRect(_selector->getSurface(), &r, Palette::blockOffset(1));
			r.w -= 2;
			r.h -= 2;
			r.x++;
			r.y++;
			SDL_FillRect(_selector->getSurface(), &r, 0);
		}
		else
		{
			r.w = _selector->getWidth();
			r.h = _selector->getHeight();
			r.x = 0;
			r.y = 0;
			SDL_FillRect(_selector->getSurface(), &r, 0);
		}
	}
}

/**
 * Draws the view of all the facilities in the base, connectors
 * between them and crafts landed in hangars.
 */
void BaseView::draw()
{
	// Draw grid squares
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			_texture->getFrame(0)->setX(x * GRID_SIZE);
			_texture->getFrame(0)->setY(y * GRID_SIZE);
			_texture->getFrame(0)->blit(this);
		}
	}

	vector<Craft*>::iterator craft = _base->getCrafts()->begin();
	
	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		// Draw facility shape
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{
				Surface *frame;

				if ((*i)->getBuildTime() == 0)
					frame = _texture->getFrame((*i)->getRules()->getSpriteShape() + num);
				else
					frame = _texture->getFrame((*i)->getRules()->getSpriteShape() + num + 2 + (*i)->getRules()->getSize());

				frame->setX(x * GRID_SIZE);
				frame->setY(y * GRID_SIZE);
				frame->blit(this);

				num++;
			}
		}
	}

	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		// Draw connectors
		if ((*i)->getBuildTime() == 0)
		{
			// Facilities to the right
			int x = (*i)->getX() + (*i)->getRules()->getSize();
			if (x < BASE_SIZE)
			{
				for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
				{
					if (_facilities[x][y] != 0 && _facilities[x][y]->getBuildTime() == 0)
					{
						_texture->getFrame(7)->setX(x * GRID_SIZE - GRID_SIZE / 2);
						_texture->getFrame(7)->setY(y * GRID_SIZE);
						_texture->getFrame(7)->blit(this);
					}
				}
			}

			// Facilities to the bottom
			int y = (*i)->getY() + (*i)->getRules()->getSize();
			if (y < BASE_SIZE)
			{
				for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
				{
					if (_facilities[x][y] != 0 && _facilities[x][y]->getBuildTime() == 0)
					{
						_texture->getFrame(8)->setX(x * GRID_SIZE);
						_texture->getFrame(8)->setY(y * GRID_SIZE - GRID_SIZE / 2);
						_texture->getFrame(8)->blit(this);
					}
				}
			}
		}
	}

	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		// Draw facility graphic
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{
				if ((*i)->getRules()->getSize() == 1)
				{
					_texture->getFrame((*i)->getRules()->getSpriteFacility() + num)->setX(x * GRID_SIZE);
					_texture->getFrame((*i)->getRules()->getSpriteFacility() + num)->setY(y * GRID_SIZE);
					_texture->getFrame((*i)->getRules()->getSpriteFacility() + num)->blit(this);
				}

				num++;
			}
		}

		// Draw crafts
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getCrafts() > 0 && craft != _base->getCrafts()->end())
		{
			_texture->getFrame((*craft)->getRules()->getSprite())->setX((*i)->getX() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2);
			_texture->getFrame((*craft)->getRules()->getSprite())->setY((*i)->getY() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2);
			_texture->getFrame((*craft)->getRules()->getSprite())->blit(this);
			craft++;
		}

		// Draw time remaining
		if ((*i)->getBuildTime() > 0)
		{
			Text *text = new Text(_big, _small, GRID_SIZE * (*i)->getRules()->getSize(), 16, 0, 0);
			text->setPalette(getPalette());
			text->setX((*i)->getX() * GRID_SIZE);
			text->setY((*i)->getY() * GRID_SIZE + (GRID_SIZE * (*i)->getRules()->getSize() - 16) / 2);
			text->setBig();
			stringstream ss;
			ss << (*i)->getBuildTime();
			text->setAlign(ALIGN_CENTER);
			text->setColor(Palette::blockOffset(13)+5);
			text->setText(ss.str());
			text->blit(this);
		}
	}
}

/**
 * Blits the base view and selector.
 * @param surface Pointer to surface to blit onto.
 */
void BaseView::blit(Surface *surface)
{
	Surface::blit(surface);
	if (_selector != 0)
		_selector->blit(surface);
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mousePress(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseRelease(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseClick(ev, scale, state);
	}
}

/**
 * Selects the facility the mouse is over.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mouseOver(SDL_Event *ev, int scale, State *state)
{
	double x = ev->button.x - _x * scale;
	double y = ev->button.y - _y * scale;
	_gridX = (int)floor(x / (GRID_SIZE * scale));
	_gridY = (int)floor(y / (GRID_SIZE * scale));
	if (_gridX >= 0 && _gridX < BASE_SIZE && _gridY >= 0 && _gridY < BASE_SIZE)
	{
		_selFacility = _facilities[_gridX][_gridY];
		if (_selSize > 0)
		{
			if (_gridX + _selSize - 1 < BASE_SIZE && _gridY + _selSize - 1 < BASE_SIZE)
			{
				_selector->setX(_x + _gridX * GRID_SIZE);
				_selector->setY(_y + _gridY * GRID_SIZE);
				_selector->setVisible(true);
			}
			else
			{
				_selector->setVisible(false);
			}
		}
	}
	else
	{
		_selFacility = 0;
		if (_selSize > 0)
		{
			_selector->setVisible(false);
		}
	}

	InteractiveSurface::mouseOver(ev, scale, state);
}

/**
 * Deselects the facility.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mouseOut(SDL_Event *ev, int scale, State *state)
{
	_selFacility = 0;
	if (_selSize > 0)
	{
		_selector->setVisible(false);
	}

	InteractiveSurface::mouseOut(ev, scale, state);
}