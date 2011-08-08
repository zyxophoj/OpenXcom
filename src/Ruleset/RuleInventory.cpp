/*
 * Copyright 2010 OpenXcom Developers.
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
#include "RuleInventory.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of inventory section.
 * @param id String defining the id.
 */
RuleInventory::RuleInventory(std::string id): _id(id), _x(0), _y(0), _tus(0), _type(INV_SLOT), _slots()
{
}

RuleInventory::~RuleInventory()
{
}

/**
 * Returns the language string that names
 * this inventory section. Each section has a unique name.
 * @return Section name.
 */
std::string RuleInventory::getId() const
{
	return _id;
}

/**
 * Returns the X position of the inventory section on the screen.
 * @return Position in pixels.
 */
int RuleInventory::getX() const
{
	return _x;
}

/**
 * Changes the X position of the inventory section on the screen.
 * @param x Position in pixels.
 */
void RuleInventory::setX(int x)
{
	_x = x;
}

/**
 * Returns the Yposition of the inventory section on the screen.
 * @return Position in pixels.
 */
int RuleInventory::getY() const
{
	return _y;
}

/**
 * Changes the X position of the inventory section on the screen.
 * @param y Position in pixels.
 */
void RuleInventory::setY(int y)
{
	_y = y;
}

/**
 * Returns the type of the inventory section.
 * Slot-based contain a limited number of slots.
 * Hands only contain one slot but can hold any item.
 * Ground can hold infinite items but don't attach to soldiers.
 * @return The inventory type.
 */
InventoryType RuleInventory::getType() const
{
	return _type;
}

/**
 * Changes the type of the inventory section.
 * @param type The inventory type.
 */
void RuleInventory::setType(InventoryType type)
{
	_type = type;
}

/**
 * Adds a storage slot to the inventory section.
 * Each slot can only be occupied by one item (or part of one).
 * @param x X position in the grid.
 * @param y Y position in the grid.
 */
void RuleInventory::addSlot(int x, int y)
{
	struct RuleSlot s = {x, y};
	_slots.push_back(s);
}

/**
 * Gets all the slots in the inventory section.
 * @return List of slots.
 */
std::vector<struct RuleSlot> *const RuleInventory::getSlots()
{
	return &_slots;
}

/**
 * Gets the slot located in the specified mouse position.
 * @param x Mouse X position. Returns the slot's X position.
 * @param y Mouse Y position. Returns the slot's Y position.
 * @return True if there's a slot there, False otherwise.
 */
bool RuleInventory::checkSlotInPosition(int *x, int *y) const
{
	int mouseX = *x, mouseY = *y;
	if (_type == INV_HAND)
	{
		for (int xx = 0; xx < HAND_W; ++xx)
		{
			for (int yy = 0; yy < HAND_H; ++yy)
			{
				if (mouseX >= _x + xx * SLOT_W && mouseX < _x + (xx + 1) * SLOT_W &&
					mouseY >= _y + yy * SLOT_H && mouseY < _y + (yy + 1) * SLOT_H)
				{
					*x = 0;
					*y = 0;
					return true;
				}
			}
		}
	}
	else
	{
		for (std::vector<RuleSlot>::const_iterator i = _slots.begin(); i != _slots.end(); ++i)
		{
			if (mouseX >= _x + i->x * SLOT_W && mouseX < _x + (i->x + 1) * SLOT_W &&
				mouseY >= _y + i->y * SLOT_H && mouseY < _y + (i->y + 1) * SLOT_H)
			{
				*x = i->x;
				*y = i->y;
				return true;
			}
		}
	}
	return false;
}

/**
 * Checks if an item completely fits when
 * placed in a certain slot.
 * @param item 
 * @return True if there's a slot there, False otherwise.
 */
bool RuleInventory::fitItemInSlot(BattleItem *item) const
{
	// TODO
	return true;
}

}