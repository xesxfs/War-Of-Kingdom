/* $Id: panel.hpp 54007 2012-04-28 19:16:10Z mordante $ */
/*
   Copyright (C) 2008 - 2012 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_WIDGETS_PANEL_HPP_INCLUDED
#define GUI_WIDGETS_PANEL_HPP_INCLUDED

#include "gui/widgets/container.hpp"

namespace gui2 {

/**
 * Visible container to hold multiple widgets.
 *
 * This widget can draw items beyond the widgets it holds and in front of them.
 * A panel is always active so these functions return dummy values.
 */
class tpanel : public tcontainer_
{

public:

	/**
	 * Constructor.
	 *
	 * @param canvas_count        The canvas count for tcontrol.
	 */
	explicit tpanel(const unsigned canvas_count = 2) ;

	/**
	 * Returns the client rect.
	 *
	 * The client rect is the area which is used for child items. The rest of
	 * the area of this widget is used for its own decoration.
	 *
	 * @returns                   The client rect.
	 */
	virtual SDL_Rect get_client_rect() const;

	/** Inherited from tcontrol. */
	bool get_active() const { return true; }

	/** Inherited from tcontrol. */
	unsigned get_state() const { return 0; }

	bool exist_anim();

	void set_hole_variable(int left, int right);

	/** Inherited from tcontrol. */
	void update_canvas();

	/** Inherited from tcontrol. */
	void impl_draw_background(
			  surface& frame_buffer
			, int x_offset
			, int y_offset);

	/** Inherited from tcontrol. */
	void impl_draw_foreground(
			  surface& frame_buffer
			, int x_offset
			, int y_offset);
private:
	/** Inherited from tcontrol. */
	const std::string& get_control_type() const;

	/** Inherited from tcontainer_. */
	tpoint border_space() const;

	int hole_left_;
	int hole_right_;

	/** Inherited from tcontainer_. */
	void set_self_active(const bool /*active*/) {}

};

} // namespace gui2

#endif


