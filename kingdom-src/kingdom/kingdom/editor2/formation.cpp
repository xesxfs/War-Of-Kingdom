#ifndef _ROSE_EDITOR

#define GETTEXT_DOMAIN "editor-lib"

#include "global.hpp"
#include "rose_config.hpp"
#include "loadscreen.hpp"
#include "DlgCoreProc.hpp"
#include "gettext.hpp"
#include "serialization/binary_or_text.hpp"
#include "stdafx.h"
#include <windowsx.h>
#include <string.h>

#include "resource.h"

#include "xfunc.h"
#include "win32x.h"
#include "struct.h"

#include "map.hpp"
#include <boost/foreach.hpp>

namespace ns {
	int clicked_formation;
	
	int action_formation;
}

void tformation::from_config(const config& cfg)
{
	formation_from_cfg_ = *this;
}

void tformation::from_ui_formation_edit(HWND hdlgP)
{
}

void tformation::update_to_ui_formation_edit(HWND hdlgP) const
{
}

void tformation::update_to_ui_row(HWND hdlgP) const
{
}

std::string tformation::generate() const
{
	std::stringstream strstr;

	strstr << "[formation]\n";
	strstr << "[/formation]\n";
	strstr << "\n";

	return strstr.str();
}

void tcore::update_to_ui_formation(HWND hdlgP)
{
	char text[_MAX_PATH];
	std::stringstream strstr;

	HWND hctl = GetDlgItem(hdlgP, IDC_TV_FORMATION_EXPLORER);

	// 1. clear treeview
	TreeView_DeleteAllItems(hctl);

	// 2. fill content
	htvroot_formation_ = TreeView_AddLeaf(hctl, TVI_ROOT);
	strstr.str("");
	strstr << dgettext_2_ansi("kingdom-lib", "tactical^Formation");
	strcpy(text, strstr.str().c_str());
	// must set TVIF_CHILDREN
	TreeView_SetItem1(hctl, htvroot_formation_, TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN, 0, 0, 0, 
		1, text);

	HTREEITEM htvi_formation, htvi_branch, htvi;
	utils::string_map symbols;

	const std::map<int, tformation_profile>& formations = unit_types.formations();
	for (std::map<int, tformation_profile>::const_iterator it = formations.begin(); it != formations.end(); ++ it) {
		const tformation_profile& f = it->second;

		htvi_formation = TreeView_AddLeaf(hctl, htvroot_formation_);
		LPARAM lParam = f.index();
		strstr.str("");
		strstr << std::setw(2) << std::setfill('0') << f.index() << ": " << utf8_2_ansi(f.name().c_str()) << "(" << f.id() << ")";
		strstr << utf8_2_ansi(_("Description")) << ": ";
		strstr << utf8_2_ansi(f.description().c_str());
		strcpy(text, strstr.str().c_str());
		TreeView_SetItem2(hctl, htvi_formation, TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_CHILDREN, lParam, gdmgr._iico_dir, gdmgr._iico_dir, 1, text);

		// arms requrie
		htvi = TreeView_AddLeaf(hctl, htvi_formation);
		strstr.str("");
		strstr << _("Arms require");
		strcpy(text, utf8_2_ansi(strstr.str().c_str()));
		TreeView_SetItem2(hctl, htvi, TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN, 0, gdmgr._iico_txt, gdmgr._iico_txt, 1, text);

		htvi_branch = htvi;

		htvi = TreeView_AddLeaf(hctl, htvi_branch);
		strstr.str("");
		strstr << _("Type") << ": ";
		for (std::set<std::string>::const_iterator it2 = f.arms_type_.begin(); it2 != f.arms_type_.end(); ++ it2) {
			if (it2 != f.arms_type_.begin()) {
				strstr << ", ";
			}
			strstr << dsgettext("kingdom-lib", it2->c_str());
		}

		strstr << "     ";
		strstr << _("attack^Range") << ": " << dsgettext("kingdom-lib", unit_types.range_ids()[f.arms_range_].c_str());
		strstr << "(" << f.arms_range_id_ << ")";
		
		strstr << "     ";
		strstr << _("Quantity") << ": " << f.arms_count_;
		strcpy(text, utf8_2_ansi(strstr.str().c_str()));
		TreeView_SetItem2(hctl, htvi, TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN, 0, gdmgr._iico_txt, gdmgr._iico_txt, 0, text);

		TreeView_Expand(hctl, htvi_branch, TVE_EXPAND);

		if (f.attack_flag_) {
			bool first = true;
			htvi = TreeView_AddLeaf(hctl, htvi_formation);
			strstr.str("");
			strstr << _("Attack") << ": ";
			if (f.attack_flag_ & tformation_profile::SLOW) {
				if (!first) {
					strstr << ", ";
				} else {
					first = false;
				}
				strstr << dsgettext("kingdom-lib", "slowed");
			}
			if (f.attack_flag_ & tformation_profile::BREAK) {
				if (!first) {
					strstr << ", ";
				} else {
					first = false;
				}
				strstr << dsgettext("kingdom-lib", "broken");
			}
			if (f.attack_flag_ & tformation_profile::POISON) {
				if (!first) {
					strstr << ", ";
				} else {
					first = false;
				}
				strstr << dsgettext("kingdom-lib", "poisoned");
			}
			strcpy(text, utf8_2_ansi(strstr.str().c_str()));
			TreeView_SetItem2(hctl, htvi, TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN, 0, gdmgr._iico_txt, gdmgr._iico_txt, 0, text);
		}

		htvi = TreeView_AddLeaf(hctl, htvi_formation);
		strstr.str("");
		strstr << _("Icon") << "/" << _("Image") << ": ";
		strstr << f.icon() << "        " << f.bg_image();
		strcpy(text, utf8_2_ansi(strstr.str().c_str()));
		TreeView_SetItem2(hctl, htvi, TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN, 0, gdmgr._iico_txt, gdmgr._iico_txt, 0, text);

		TreeView_Expand(hctl, htvi_formation, TVE_EXPAND);
	}

	TreeView_Expand(hctl, htvroot_formation_, TVE_EXPAND);
}

bool tcore::formations_dirty() const
{
	return ns::core.formations_updating_ != ns::core.formations_from_cfg_;
}

bool tformation::dirty() const
{
	if (*this != formation_from_cfg_) {
		return true;
	}
	return false;
}

BOOL On_DlgFormationInitDialog(HWND hdlgP, HWND hwndFocus, LPARAM lParam)
{
	HWND hwndParent = GetParent(hdlgP); 
    DLGHDR *pHdr = (DLGHDR *) GetWindowLong(hwndParent, GWL_USERDATA);
    SetWindowPos(hdlgP, HWND_TOP, pHdr->rcDisplay.left, pHdr->rcDisplay.top, 0, 0, SWP_NOSIZE); 

	Static_SetText(GetDlgItem(hdlgP, IDC_STATIC_CANDIDATE), utf8_2_ansi(_("Candidate")));
	Static_SetText(GetDlgItem(hdlgP, IDC_STATIC_OFFICE), utf8_2_ansi(_("Office")));
	Static_SetText(GetDlgItem(hdlgP, IDC_STATIC_WANDER), utf8_2_ansi(_("Wander")));

	return FALSE;
}

BOOL On_DlgFormationEditInitDialog(HWND hdlgP, HWND hwndFocus, LPARAM lParam)
{
	editor_config::move_subcfg_right_position(hdlgP, lParam);

	std::stringstream strstr;
	if (ns::action_formation == ma_edit) {
		strstr << _("Edit formaton");
	} else {
		strstr << _("Add formaton");
	}
	SetWindowText(hdlgP, utf8_2_ansi(strstr.str().c_str()));

	Static_SetText(GetDlgItem(hdlgP, IDC_STATIC_LEADER), utf8_2_ansi(_("Leader")));
	Static_SetText(GetDlgItem(hdlgP, IDC_STATIC_CITY), utf8_2_ansi(_("City")));
	
	tformation& f = ns::core.formations_updating_[ns::clicked_formation];
	f.update_to_ui_formation_edit(hdlgP);

	return FALSE;
}

void On_DlgFormationEditCommand(HWND hdlgP, int id, HWND hwndCtrl, UINT codeNotify)
{
	BOOL changed = FALSE;

	switch (id) {
	case IDOK:
		changed = TRUE;
		ns::core.formations_updating_[ns::clicked_formation].from_ui_formation_edit(hdlgP);
	case IDCANCEL:
		EndDialog(hdlgP, changed? 1: 0);
		break;
	}
}

BOOL CALLBACK DlgFormationEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
	case WM_INITDIALOG:
		return On_DlgFormationEditInitDialog(hDlg, (HWND)(wParam), lParam);
	HANDLE_MSG(hDlg, WM_COMMAND, On_DlgFormationEditCommand);
	HANDLE_MSG(hDlg, WM_DRAWITEM, editor_config::On_DlgDrawItem);
	}
	
	return FALSE;
}

void OnFormationAddBt(HWND hdlgP)
{
	ns::core.formations_updating_.push_back(tformation());
	ns::clicked_formation = ns::core.formations_updating_.size() - 1;

	ns::core.update_to_ui_formation(hdlgP);
	ns::core.set_dirty(tcore::BIT_FORMATION, ns::core.formations_dirty()); 
}

void OnFormationDelBt(HWND hdlgP)
{
	std::vector<tformation>::iterator it = ns::core.formations_updating_.begin();
	std::advance(it, ns::clicked_formation);
	ns::core.formations_updating_.erase(it);

	int clicked = ns::clicked_formation;
	if (clicked >= (int)ns::core.formations_updating_.size()) {
		clicked = -1;
	}
	ns::core.update_to_ui_formation(hdlgP);
	ns::core.set_dirty(tcore::BIT_FORMATION, ns::core.formations_dirty());
}

void OnFormationEditBt(HWND hdlgP)
{
	RECT rcBtn;
	LPARAM lParam;
	
	GetWindowRect(GetDlgItem(hdlgP, IDC_TV_FORMATION_EXPLORER), &rcBtn);
	lParam = posix_mku32((rcBtn.left > 0)? rcBtn.left: rcBtn.right, rcBtn.top);

	ns::action_formation = ma_edit;
/*
	if (DialogBoxParam(gdmgr._hinst, MAKEINTRESOURCE(IDD_FORMATIOMEDIT), hdlgP, DlgFactionEditProc, lParam)) {
		ns::core.update_to_ui_formation(hdlgP, ns::clicked_formation);
		ns::core.set_dirty(tcore::BIT_FORMATION, ns::core.formations_dirty());
	}
*/
	return;
}

void On_DlgFormationCommand(HWND hdlgP, int id, HWND hwndCtrl, UINT codeNotify)
{
	tformation& f = ns::core.formations_updating_[ns::clicked_formation];

	switch (id) {
	case IDM_ADD:
		OnFormationAddBt(hdlgP);
		break;
	case IDM_DELETE:
		OnFormationDelBt(hdlgP);
		break;
	case IDM_EDIT:
		OnFormationEditBt(hdlgP);
		break;
	}

	return;
}

void formation_notify_handler_rclick(HWND hdlgP, int DlgItem, LPNMHDR lpNMHdr)
{
	LVITEM					lvi;
	LPNMITEMACTIVATE		lpnmitem;
	int						icount;

	if (lpNMHdr->idFrom != IDC_TV_FORMATION_EXPLORER) {
		return;
	}

	// NM_表示对通用控件都通用,范围丛(0, 99)
	// TVN_表示只能TreeView通用,范围丛(400, 499)
	lpnmitem = (LPNMITEMACTIVATE)lpNMHdr;
	// 如果单击到的是复选框位置,把复选框状态置反
	// 当前定义的图标大小是16x16. ptAction反回的(x,y)是整个列表视图内的坐标,因而y值不大好判断
	// 认为如果x是小于16的就认为是击中复选框
	
	POINT point = {lpnmitem->ptAction.x, lpnmitem->ptAction.y};
	MapWindowPoints(lpNMHdr->hwndFrom, NULL, &point, 1);

	lvi.iItem = lpnmitem->iItem;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iSubItem = 0;
	lvi.pszText = gdmgr._menu_text;
	lvi.cchTextMax = _MAX_PATH;
	ListView_GetItem(lpNMHdr->hwndFrom, &lvi);

	icount = ListView_GetItemCount(lpNMHdr->hwndFrom);

	if (lpNMHdr->idFrom == IDC_TV_FORMATION_EXPLORER) {
		EnableMenuItem(gdmgr._hpopup_editor, IDM_ADD, MF_BYCOMMAND);
		if (lpnmitem->iItem < 0) {
			EnableMenuItem(gdmgr._hpopup_editor, IDM_EDIT, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(gdmgr._hpopup_editor, IDM_DELETE, MF_BYCOMMAND | MF_GRAYED);
		}

		TrackPopupMenuEx(gdmgr._hpopup_editor, 0, 
			point.x, 
			point.y, 
			hdlgP, 
			NULL);

		EnableMenuItem(gdmgr._hpopup_editor, IDM_ADD, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(gdmgr._hpopup_editor, IDM_EDIT, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(gdmgr._hpopup_editor, IDM_DELETE, MF_BYCOMMAND | MF_ENABLED);

		if (lpnmitem->iItem >= 0) {
			ns::clicked_formation = lpnmitem->iItem;
		}
		return;

	} 
	ns::type = DlgItem;
	ns::clicked_hero = lvi.lParam;

    return;
}

void formation_notify_handler_dblclk(HWND hdlgP, int DlgItem, LPNMHDR lpNMHdr)
{
	LVITEM					lvi;
	LPNMITEMACTIVATE		lpnmitem;

	// NM_表示对通用控件都通用,范围丛(0, 99)
	// TVN_表示只能TreeView通用,范围丛(400, 499)
	lpnmitem = (LPNMITEMACTIVATE)lpNMHdr;
	// 如果单击到的是复选框位置,把复选框状态置反
	// 当前定义的图标大小是16x16. ptAction反回的(x,y)是整个列表视图内的坐标,因而y值不大好判断
	// 认为如果x是小于16的就认为是击中复选框
	
	lvi.iItem = lpnmitem->iItem;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iSubItem = 0;
	lvi.pszText = gdmgr._menu_text;
	lvi.cchTextMax = _MAX_PATH;
	ListView_GetItem(lpNMHdr->hwndFrom, &lvi);

	if (lpnmitem->iItem >= 0) {
		if (lpNMHdr->idFrom == IDC_TV_FORMATION_EXPLORER) {
			ns::clicked_formation = lpnmitem->iItem;
			OnFormationEditBt(hdlgP);
		}
	}
    return;
}

BOOL On_DlgFormationNotify(HWND hdlgP, int DlgItem, LPNMHDR lpNMHdr)
{
	if (lpNMHdr->code == NM_RCLICK) {
		formation_notify_handler_rclick(hdlgP, DlgItem, lpNMHdr);
	} else if (lpNMHdr->code == NM_DBLCLK) {
		formation_notify_handler_dblclk(hdlgP, DlgItem, lpNMHdr);
	}
	return FALSE;
}

BOOL CALLBACK DlgFormationProc(HWND hdlgP, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_INITDIALOG:
		return On_DlgFormationInitDialog(hdlgP, (HWND)(wParam), lParam);
	HANDLE_MSG(hdlgP, WM_COMMAND, On_DlgFormationCommand);
	HANDLE_MSG(hdlgP, WM_NOTIFY,  On_DlgFormationNotify);
	}
	
	return FALSE;
}

#endif