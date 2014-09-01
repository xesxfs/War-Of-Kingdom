/* $Id: editor_palettes.hpp 47812 2010-12-05 18:08:26Z mordante $ */
/*
  Copyright (C) 2003 - 2010 by David White <dave@whitevine.net>
  Part of the Battle for Wesnoth Project http://www.wesnoth.org/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY.

  See the COPYING file for more details.
*/

/**
 * Manage the terrain-palette in the editor.
 * Note: this is a near-straight rip from the old editor.
*/

#ifndef EDITOR_PALETTES_H_INCLUDED
#define EDITOR_PALETTES_H_INCLUDED

#include "../display.hpp"
#include "brush.hpp"
#include "editor_layout.hpp"

namespace editor {
/**
 * Stores the info about the data in editor-groups.cfg in a nice format.
 *
 *  Helper struct which for some reason can't be moved to the cpp file.
 */
struct terrain_group
{
	terrain_group(const config& cfg, display& gui);

	std::string id;
	std::string icon;
	t_string name;
    bool core;
};

/** Palette where the terrain to be drawn can be selected. */
class terrain_palette : public gui::widget {
public:
	terrain_palette(display &gui, const size_specs &sizes,
					const config& cfg,
					t_translation::t_terrain& fore,
					t_translation::t_terrain& back);

	const gamemap& map() const { return gui_.get_map(); }

	/** Scroll the terrain-palette up one step if possible. */
	void scroll_up();

	/** Scroll the terrain-palette down one step if possible. */
	void scroll_down();

	/** Scroll the terrain-palette to the top. */
	void scroll_top();

	/** Scroll the terrain-palette to the bottom. */
	void scroll_bottom();

	/**
	 * Sets a group active id == terrain_map_->first
	 *
	 * The selected terrains remain the same, this can result in no visible
	 * selected items.
	 */
	void set_group(const std::string& id);

	/** Return the currently selected foreground terrain. */
	t_translation::t_terrain selected_fg_terrain() const;
	/** Return the currently selected background terrain. */
	t_translation::t_terrain selected_bg_terrain() const;

	void swap();

	/** Select a foreground terrain. */
	void select_fg_terrain(t_translation::t_terrain);
	void select_bg_terrain(t_translation::t_terrain);

	/**
	 * Draw the palette.
	 *
	 * If force is true everything will be redrawn,
	 * even though it is not invalidated.
	 */
	void draw(bool force=false);
	virtual void draw();
	virtual void handle_event(const SDL_Event& event);
	void set_dirty(bool dirty=true);

	/** Return the number of terrains in the palette. */
	size_t num_terrains() const;

	/**
	 * Update the size of this widget.
	 *
	 * Use if the size_specs have changed.
	 */
	void adjust_size();

	/** Sets the tooltips used in the palette */
	void load_tooltips();

	const std::vector<terrain_group>& terrain_groups() const { return terrain_groups_; }
	const std::string& current_group_id() const { return current_group_id_; }

private:
	void draw_old(bool);

	/**
	 * To be called when a mouse click occurs.
	 *
	 * Check if the coordinates is a terrain that may be chosen,
	 * and select the terrain if that is the case.
	 */
	void left_mouse_click(const int mousex, const int mousey);
	void right_mouse_click(const int mousex, const int mousey);


	/** Return the number of the tile that is at coordinates (x, y) in the panel. */
	int tile_selected(const int x, const int y) const;

	/** Return a string represeting the terrain and the underlying ones. */
	std::string get_terrain_string(const t_translation::t_terrain);

	/** Update the report with the currently selected terrains. */
	void update_report();

	const size_specs &size_specs_;
	display &gui_;
	unsigned int tstart_;

	/**
	 * This map contains all editor_group as defined in terrain.cfg
	 * and associate with the group there.
	 * The group 'all' is added automatically, and all terrains
	 * are also automatically stored in this group.
	 */
	std::map<std::string, t_translation::t_list> terrain_map_;

	/** A copy from the terrain_map_->second for the current active group. */
	t_translation::t_list terrains_;

	std::string current_group_id_;

	/**
	 * The editor_groups as defined in editor-groups.cfg.
	 *
	 * Note the user must make sure the id's here are the same as the
	 * editor_group in terrain.cfg.
	 */
	std::vector<terrain_group> terrain_groups_;

    std::set<t_translation::t_terrain> non_core_terrains_;

	/**
	 * The group buttons behave like a radio group.
	 *
	 * This one points to the selected button, this value should not be 0
	 * otherwise things will fail. Thus should be set in constructor.
	 */

	size_t nterrains_, terrain_start_;
	t_translation::t_terrain& selected_fg_terrain_;
	t_translation::t_terrain& selected_bg_terrain_;

};

} //end namespace editor
#endif // EDITOR_PALETTES_H_INCLUDED

