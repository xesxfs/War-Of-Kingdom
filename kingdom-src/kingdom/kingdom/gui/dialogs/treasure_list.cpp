/* $Id: hero_list.cpp 49598 2011-05-22 17:55:54Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Jörg Hinrichs <joerg.hinrichs@alice-dsl.de>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "kingdom-lib"

#include "gui/dialogs/treasure_list.hpp"

#include "formula_string_utils.hpp"
#include "gettext.hpp"
#include "team.hpp"
#include "artifical.hpp"
#include "gui/dialogs/helper.hpp"
#include "gui/widgets/button.hpp"
#include "gui/widgets/image.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/toggle_button.hpp"
#include "gui/widgets/toggle_panel.hpp"
#include "gui/widgets/scroll_label.hpp"
#include "gui/widgets/stacked_widget.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "play_controller.hpp"
#include "resources.hpp"

#include <boost/bind.hpp>

namespace gui2 {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_game_load
 *
 * == Load a game ==
 *
 * This shows the dialog to select and load a savegame file.
 *
 * @begin{table}{dialog_widgets}
 *
 * txtFilter & & text & m &
 *         The filter for the listbox items. $
 *
 * savegame_list & & listbox & m &
 *         List of savegames. $
 *
 * -filename & & control & m &
 *         Name of the savegame. $
 *
 * -date & & control & o &
 *         Date the savegame was created. $
 *
 * preview_pane & & widget & m &
 *         Container widget or grid that contains the items for a preview. The
 *         visible status of this container depends on whether or not something
 *         is selected. $
 *
 * -minimap & & minimap & m &
 *         Minimap of the selected savegame. $
 *
 * -imgLeader & & image & m &
 *         The image of the leader in the selected savegame. $
 *
 * -lblScenario & & label & m &
 *         The name of the scenario of the selected savegame. $
 *
 * -lblSummary & & label & m &
 *         Summary of the selected savegame. $
 *
 * @end{table}
 */

REGISTER_DIALOG(treasure_list)

ttreasure_list::ttreasure_list(std::vector<team>& teams, unit_map& units, hero_map& heros, int side)
	: teams_(teams)
	, units_(units)
	, heros_(heros)
	, side_(side)
	, ownerships_()
	, sorting_widgets_()
	, sorting_widget_(NULL)
	, current_page_(twidget::npos)
{
}

void ttreasure_list::pre_show(CVideo& /*video*/, twindow& window)
{
	tlabel* label = find_widget<tlabel>(&window, "title", false, true);
	if (side_ >= 1) {
		label->set_label(_("Side Treasure List"));
	} else {
		label->set_label(_("Treasure List"));
	}

	page_panel_ = find_widget<tstacked_widget>(&window, "page", false, true);

	lists_.push_back(find_widget<tlistbox>(&window, "ownership_list", false, true));
	for (std::vector<tlistbox*>::const_iterator it = lists_.begin(); it != lists_.end(); ++ it) {
		tlistbox& list = **it;
		list.set_callback_value_change(dialog_callback3<ttreasure_list, tlistbox, &ttreasure_list::hero_changed>);
	}

	std::map<int, ownership>::iterator find_it;

	for (size_t i = 0; i < teams_.size(); i ++) {
		if (side_ != -1 && i != side_ - 1) {
			continue;
		}
		team& current_team = teams_[i];
		const std::vector<size_t>& holded_treasures = current_team.holded_treasures();
		for (std::vector<size_t>::const_iterator it = holded_treasures.begin(); it != holded_treasures.end(); ++ it) {
			size_t t = *it;
			find_it = ownerships_.find(t);
			if (find_it != ownerships_.end()) {
				find_it->second.sides_.push_back(&current_team);
			} else {
				ownership owner;
				owner.sides_.push_back(&current_team);
				ownerships_[t] = owner;
			}
		}
		
	}
	for (hero_map::iterator it = heros_.begin(); it != heros_.end(); ++ it) {
		hero& h = *it;
		if (side_ != -1 && h.side_ != side_ - 1) {
			continue;
		}
		if (h.treasure_ != HEROS_NO_TREASURE) {
			find_it = ownerships_.find(h.treasure_);
			if (find_it != ownerships_.end()) {
				find_it->second.heros_.push_back(&h);
			} else {
				ownership owner;
				owner.heros_.push_back(&h);
				ownerships_[h.treasure_] = owner;
			}
		}
	}
/*
	// hide treasure
	const std::vector<int>& hide_treasures = resources::controller->treasures();
	for (std::vector<int>::const_iterator it = hide_treasures.begin(); it != hide_treasures.end(); ++ it) {
		size_t t = *it;
		find_it = ownerships_.find(t);
		if (find_it != ownerships_.end()) {
			find_it->second.sides_.push_back(&teams_[0]);
		} else {
			ownership owner;
			owner.sides_.push_back(&teams_[0]);
			ownerships_[t] = owner;
		}
	}
*/
	catalog_page(window, OWNERSHIP_PAGE, false);

/*
	connect_signal_mouse_left_click(
		find_widget<tbutton>(&window, "ownership", false)
		, boost::bind(
			&ttreasure_list::catalog_page
			, this
			, boost::ref(window)
			, (int)OWNERSHIP_PAGE
			, true));
*/
}

void ttreasure_list::post_show(twindow& window)
{
}

void ttreasure_list::hero_changed(twindow& window, tlistbox& list, const int type)
{
	int selected_row = list.get_selected_row();
	if (selected_row < 0) {
		return;
	}
	twidget* grid_ptr = list.get_row_panel(selected_row);
	unsigned int tid = dynamic_cast<ttoggle_panel*>(grid_ptr)->get_data();
	
	twindow::tinvalidate_layout_blocker invalidate_layout_blocker(window);

	tcontrol* portrait = find_widget<tcontrol>(&window, "portrait", false, true);
	tscroll_label* biography = find_widget<tscroll_label>(&window, "biography", false, true);

	std::stringstream str;
	str << "treasure/" << tid << ".png";
	portrait->set_label(str.str());
	biography->set_label("");
}

void ttreasure_list::fill_table(tlistbox& list, int catalog)
{
	for (std::map<int, ownership>::iterator it = ownerships_.begin(); it != ownerships_.end(); ++ it) {
		fill_table_row(list, it, catalog);
	}
}

void ttreasure_list::fill_table_row(tlistbox& list, std::map<int, ownership>::iterator& p, int catalog)
{
	std::vector<int> features;
	std::stringstream str;

	/*** Add list item ***/
	string_map table_item;
	std::map<std::string, string_map> table_item_item;

	if (catalog == OWNERSHIP_PAGE) {
		const ttreasure& t = unit_types.treasure(p->first);
		table_item["label"] = t.name();
		table_item_item.insert(std::make_pair("name", table_item));

		str.str("");
		str << hero::feature_str(t.feature());
		table_item["label"] = str.str();
		table_item_item.insert(std::make_pair("feature", table_item));

		str.str("");
		for (std::vector<team*>::iterator it = p->second.sides_.begin(); it != p->second.sides_.end(); ++ it) {
			team& current_team = **it;
			if (it == p->second.sides_.begin()) {
				str << current_team.name();
			} else {
				str << ", " << current_team.name();
			}
		}
		table_item["label"] = str.str();
		table_item_item.insert(std::make_pair("side", table_item));

		str.str("");
		for (std::vector<hero*>::iterator it = p->second.heros_.begin(); it != p->second.heros_.end(); ++ it) {
			hero& h = **it;
			if (it == p->second.heros_.begin()) {
				str << h.name();
			} else {
				str << ", " << h.name();
			}
		}
		table_item["label"] = str.str();
		table_item_item.insert(std::make_pair("hero", table_item));
	}
	list.add_row(table_item_item);

	unsigned hero_index = list.get_item_count() - 1;
	twidget* grid_ptr = list.get_row_panel(hero_index);
	ttoggle_panel* toggle = dynamic_cast<ttoggle_panel*>(grid_ptr);
	toggle->set_data(p->first);
}

void ttreasure_list::catalog_page(twindow& window, int catalog, bool swap)
{
	if (!page_panel_) {
		return;
	}
	if (current_page_ == catalog) {
		return;
	}

	unsigned int selected_row = 0;
	if (swap) {
		// because sort, order is changed.
		tlistbox& list = *lists_[current_page_];
		twidget* grid_ptr = list.get_row_panel(list.get_selected_row());
		selected_row = dynamic_cast<ttoggle_panel*>(grid_ptr)->get_data();
		list.clear();
	}

	page_panel_->set_radio_layer(catalog);

	tlistbox& list = *lists_[catalog];
	fill_table(list, catalog);

	if (swap) {
		list.select_row(selected_row);
		list.invalidate_layout(true);
		// swap to other page, there is no sorted column.
		if (sorting_widget_) {
			sorting_widget_->set_sort(tbutton::NONE);
			sorting_widget_ = NULL;
		}
	} else {
		hero_changed(window, list);
	}

	if (!sorting_widgets_.count(catalog)) {
		// in order to support sortable, form relative data.
		std::vector<tbutton*> widgets;
		if (catalog == OWNERSHIP_PAGE) {
			widgets.push_back(&find_widget<tbutton>(&window, "button_name", false));
			widgets.back()->set_active(false);
			widgets.push_back(&find_widget<tbutton>(&window, "button_feature", false));
			widgets.back()->set_active(false);
			widgets.push_back(&find_widget<tbutton>(&window, "button_side", false));
			widgets.back()->set_active(false);
			widgets.push_back(&find_widget<tbutton>(&window, "button_hero", false));
			widgets.back()->set_active(false);
		}
		for (std::vector<tbutton*>::iterator i = widgets.begin(); i != widgets.end(); ++ i) {
			tbutton& widget = **i;
			connect_signal_mouse_left_click(
				widget
				, boost::bind(
					&ttreasure_list::sort_column
					, this
					, boost::ref(window)
					, boost::ref(widget)
					, catalog));
		}
		sorting_widgets_[catalog] = widgets;
	}

	// speeden compare_row, remember this catalog.
	current_page_ = catalog;
}

bool ttreasure_list::compare_row(twidget& row1, twidget& row2)
{
	unsigned int i1 = dynamic_cast<ttoggle_panel*>(&row1)->get_data();
	unsigned int i2 = dynamic_cast<ttoggle_panel*>(&row2)->get_data();

	hero* h1;
	hero* h2;

	h1 = &heros_[i1];
	h2 = &heros_[i2];

	bool result = true;
	std::vector<tbutton*>& widgets = sorting_widgets_[current_page_];

	if (current_page_ == OWNERSHIP_PAGE) {
		if (sorting_widget_ == widgets[0]) {
			// name
			result = utils::utf8str_compare(h1->name(), h2->name());
		} else if (sorting_widget_ == widgets[1]) {
			// side
			std::string str1, str2;
			if (h1->side_ != HEROS_INVALID_SIDE) {
				str1 =  teams_[h1->side_].name();
			}
			if (h2->side_ != HEROS_INVALID_SIDE) {
				str2 =  teams_[h2->side_].name();
			}
			result = utils::utf8str_compare(str1, str2);
		} else if (sorting_widget_ == widgets[2]) {
			// city
			std::string str1, str2;
			artifical* city = units_.city_from_cityno(h1->city_);
			if (city) {
				str1 = city->name();
			}
			city = units_.city_from_cityno(h2->city_);
			if (city) {
				str2 = city->name();
			}
			result = utils::utf8str_compare(str1, str2);
		} else if (sorting_widget_ == widgets[3]) {
			// loyalty
			int l1 = 0;
			int l2 = 0;
			if (h1->side_ != HEROS_INVALID_SIDE) {
				l1 = h1->loyalty(*(teams_[h1->side_].leader()));
			}
			if (h2->side_ != HEROS_INVALID_SIDE) {
				l2 = h2->loyalty(*(teams_[h2->side_].leader()));
			}
			if (l1 > l2) {
				result = false;
			}
		} else if (sorting_widget_ == widgets[4]) {
			// catalog
			if (h1->base_catalog_ > h2->base_catalog_) {
				result = false;
			}
		} else if (sorting_widget_ == widgets[5]) {
			// meritorious
			if (h1->meritorious_ > h2->meritorious_) {
				result = false;
			}
		} 

	}

	return ascend_? result: !result;
}

static bool callback_compare_row(void* caller, twidget& row1, twidget& row2)
{
	return reinterpret_cast<ttreasure_list*>(caller)->compare_row(row1, row2);
}

void ttreasure_list::sort_column(twindow& window, tbutton& widget, int catalog)
{
	if (sorting_widget_ && (sorting_widget_ != &widget)) {
		sorting_widget_->set_sort(tbutton::NONE);
	}
	// sorting_widget_ must valid before widget.set_sort
	sorting_widget_ = &widget;

	widget.set_sort(tbutton::TOGGLE);
	ascend_ = true;
	if (widget.get_sort() == tbutton::DESCEND) {
		ascend_ = false;
	}
	
	tlistbox& list = *lists_[catalog];
	list.sort(this, callback_compare_row);
	list.invalidate_layout(false);
}

} // namespace gui2