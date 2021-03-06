/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_TERRORSITE_H
#define OPENXCOM_TERRORSITE_H

#include "Target.h"
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Represents an alien terror site on the world.
 */
class TerrorSite : public Target
{
private:
	int _id;
	unsigned _secondsRemaining;
	std::string _race;
	bool _inBattlescape;
public:
	/// Creates a terror site.
	TerrorSite();
	/// Cleans up the terror site.
	~TerrorSite();
	/// Loads the terror site from YAML.
	void load(const YAML::Node& node);
	/// Saves the terror site to YAML.
	void save(YAML::Emitter& out) const;
	/// Saves the terror site's ID to YAML.
	void saveId(YAML::Emitter& out) const;
	/// Gets the terror site's ID.
	int getId() const;
	/// Sets the terror site's ID.
	void setId(int id);
	/// Gets the terror site's name.
	std::wstring getName(Language *lang) const;
	/// Gets the seconds until this terror site expires.
	unsigned getSecondsRemaining() const;
	/// Sets the seconds until this terror site expires.
	void setSecondsRemaining(unsigned seconds);
	/// Gets the terror site's alien race.
	std::string getAlienRace() const;
	/// Sets the terror site's alien race.
	void setAlienRace(const std::string &race);
	/// Sets the TerrorSite's battlescape status.
	void setInBattlescape(bool inbattle);
	/// Gets if the TerrorSite is in battlescape.
	bool isInBattlescape() const;
};

}

#endif
