/* $Id: minimap.cpp 52533 2012-01-07 02:35:17Z shadowmaster $ */
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

#define GETTEXT_DOMAIN "rose-lib"

#include "gui/auxiliary/window_builder/minimap.hpp"

#include "config.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/widgets/minimap.hpp"

namespace gui2 {

namespace implementation {

tbuilder_minimap::tbuilder_minimap(const config& cfg)
	: tbuilder_control(cfg)
	, width(cfg["width"])
	, height(cfg["height"])
{
}

twidget* tbuilder_minimap::build() const
{
	tminimap* widget = new tminimap();

	init_control(widget);

	const game_logic::map_formula_callable& size = get_screen_size_variables();

	const unsigned w = width(size);
	const unsigned h = height(size);

	if (w || h) {
		widget->set_best_size(tpoint(w, h));
	}

	DBG_GUI_G << "Window builder: placed minimap '"
			<< id << "' with definition '"
			<< definition << "'.\n";

	return widget;
}

} // namespace implementation

} // namespace gui2

/*WIKI_MACRO
 * @begin{macro}{minimap_description}
 *
 *        A minimap to show the gamemap, this only shows the map and has no
 *        interaction options. This version is used for map previews, there
 *        will be a another version which allows interaction.
 * @end{macro}
 */

/*WIKI
 * @page = GUIWidgetInstanceWML
 * @order = 2_minimap
 *
 * == Minimap ==
 *
 * @macro = minimap_description
 *
 * A minimap has no extra fields.
 * @begin{parent}{name="gui/window/resolution/grid/row/column/"}
 * @begin{tag}{name="minimap"}{min=0}{max=-1}{super="generic/widget_instance"}
 * @end{tag}{name="minimap"}
 * @end{parent}{name="gui/window/resolution/grid/row/column/"}
 */

