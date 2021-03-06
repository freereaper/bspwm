/* Copyright (c) 2012, Bastien Dejean
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bspwm.h"
#include "desktop.h"
#include "history.h"
#include "parse.h"
#include "monitor.h"
#include "window.h"
#include "tree.h"
#include "query.h"

void query_tree(FILE *rsp)
{
	fprintf(rsp, "{");
	fprintf(rsp, "\"focusedMonitorName\":\"%s\",", mon->name);
	if (pri_mon != NULL) {
		fprintf(rsp, "\"primaryMonitorName\":\"%s\",", pri_mon->name);
	}
	fprintf(rsp, "\"clientsCount\":%i,", clients_count);
	fprintf(rsp, "\"monitors\":");
	fprintf(rsp, "[");
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		query_monitor(m, rsp);
		if (m->next != NULL) {
			fprintf(rsp, ",");
		}
	}
	fprintf(rsp, "]");
	fprintf(rsp,",");
	fprintf(rsp, "\"focusHistory\":");
	query_history(rsp);
	fprintf(rsp,",");
	fprintf(rsp, "\"stackingList\":");
	query_stack(rsp);
	fprintf(rsp, "}");

}

void query_monitor(monitor_t *m, FILE *rsp)
{
	fprintf(rsp, "{");
	fprintf(rsp, "\"name\":\"%s\",", m->name);
	fprintf(rsp, "\"id\":%u,", m->id);
	fprintf(rsp, "\"randrId\":%u,", m->randr_id);
	fprintf(rsp, "\"wired\":%s,", BOOL_STR(m->wired));
	fprintf(rsp, "\"stickyCount\":%i,", m->sticky_count);
	fprintf(rsp, "\"windowGap\":%i,", m->window_gap);
	fprintf(rsp, "\"borderWidth\":%u,", m->border_width);
	fprintf(rsp, "\"padding\":");
	query_padding(m->padding, rsp);
	fprintf(rsp,",");
	fprintf(rsp, "\"rectangle\":");
	query_rectangle(m->rectangle, rsp);
	fprintf(rsp,",");
	fprintf(rsp, "\"focusedDesktopName\":\"%s\",", m->desk->name);
	fprintf(rsp, "\"desktops\":");
	fprintf(rsp, "[");
	for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
		query_desktop(d, rsp);
		if (d->next != NULL) {
			fprintf(rsp,",");
		}
	}
	fprintf(rsp, "]");
	fprintf(rsp, "}");
}

void query_desktop(desktop_t *d, FILE *rsp)
{
	fprintf(rsp, "{");
	fprintf(rsp, "\"name\":\"%s\",", d->name);
	fprintf(rsp, "\"id\":%u,", d->id);
	fprintf(rsp, "\"layout\":\"%s\",", LAYOUT_STR(d->layout));
	fprintf(rsp, "\"windowGap\":%i,", d->window_gap);
	fprintf(rsp, "\"borderWidth\":%u,", d->border_width);
	fprintf(rsp, "\"focusedNodeId\":%u,", d->focus != NULL ? d->focus->id : 0);
	fprintf(rsp, "\"padding\":");
	query_padding(d->padding, rsp);
	fprintf(rsp,",");
	fprintf(rsp, "\"root\":");
	query_node(d->root, rsp);
	fprintf(rsp, "}");
}

void query_node(node_t *n, FILE *rsp)
{
	if (n == NULL) {
		fprintf(rsp, "null");
	} else {
		fprintf(rsp, "{");
		fprintf(rsp, "\"id\":%u,", n->id);
		fprintf(rsp, "\"splitType\":\"%s\",", SPLIT_TYPE_STR(n->split_type));
		fprintf(rsp, "\"splitRatio\":%lf,", n->split_ratio);
		fprintf(rsp, "\"birthRotation\":%i,", n->birth_rotation);
		fprintf(rsp, "\"vacant\":%s,", BOOL_STR(n->vacant));
		fprintf(rsp, "\"sticky\":%s,", BOOL_STR(n->sticky));
		fprintf(rsp, "\"private\":%s,", BOOL_STR(n->private));
		fprintf(rsp, "\"locked\":%s,", BOOL_STR(n->locked));
		fprintf(rsp, "\"presel\":");
		query_presel(n->presel, rsp);
		fprintf(rsp,",");
		fprintf(rsp, "\"rectangle\":");
		query_rectangle(n->rectangle, rsp);
		fprintf(rsp,",");
		fprintf(rsp, "\"firstChild\":");
		query_node(n->first_child, rsp);
		fprintf(rsp,",");
		fprintf(rsp, "\"secondChild\":");
		query_node(n->second_child, rsp);
		fprintf(rsp,",");
		fprintf(rsp, "\"client\":");
		query_client(n->client, rsp);
		fprintf(rsp, "}");
	}
}

void query_presel(presel_t *p, FILE *rsp)
{
	if (p == NULL) {
		fprintf(rsp, "null");
	} else {
		fprintf(rsp, "{\"splitDir\":\"%s\",\"splitRatio\":%lf}", SPLIT_DIR_STR(p->split_dir), p->split_ratio);
	}
}

void query_client(client_t *c, FILE *rsp)
{
	if (c == NULL) {
		fprintf(rsp, "null");
	} else {
		fprintf(rsp, "{");
		fprintf(rsp, "\"className\":\"%s\",", c->class_name);
		fprintf(rsp, "\"instanceName\":\"%s\",", c->instance_name);
		fprintf(rsp, "\"borderWidth\":%u,", c->border_width);
		fprintf(rsp, "\"state\":\"%s\",", STATE_STR(c->state));
		fprintf(rsp, "\"lastState\":\"%s\",", STATE_STR(c->last_state));
		fprintf(rsp, "\"layer\":\"%s\",", LAYER_STR(c->layer));
		fprintf(rsp, "\"lastLayer\":\"%s\",", LAYER_STR(c->last_layer));
		fprintf(rsp, "\"urgent\":%s,", BOOL_STR(c->urgent));
		fprintf(rsp, "\"visible\":%s,", BOOL_STR(c->visible));
		fprintf(rsp, "\"tiledRectangle\":");
		query_rectangle(c->tiled_rectangle, rsp);
		fprintf(rsp,",");
		fprintf(rsp, "\"floatingRectangle\":");
		query_rectangle(c->floating_rectangle, rsp);
		fprintf(rsp, "}");
	}
}

void query_rectangle(xcb_rectangle_t r, FILE *rsp)
{
	fprintf(rsp, "{\"x\":%i,\"y\":%i,\"width\":%u,\"height\":%u}", r.x, r.y, r.width, r.height);
}

void query_padding(padding_t p, FILE *rsp)
{
	fprintf(rsp, "{\"top\":%i,\"right\":%i,\"bottom\":%i,\"left\":%i}", p.top, p.right, p.bottom, p.left);
}

void query_history(FILE *rsp)
{
	fprintf(rsp, "[");
	for (history_t *h = history_head; h != NULL; h = h->next) {
		query_coordinates(&h->loc, rsp);
		if (h->next != NULL) {
			fprintf(rsp, ",");
		}
	}
	fprintf(rsp, "]");
}

void query_coordinates(coordinates_t *loc, FILE *rsp)
{
	fprintf(rsp, "{\"monitorName\":\"%s\",\"desktopName\":\"%s\",\"nodeId\":%u}", loc->monitor->name, loc->desktop->name, loc->node!=NULL?loc->node->id:0);
}

void query_stack(FILE *rsp)
{
	fprintf(rsp, "[");
	for (stacking_list_t *s = stack_head; s != NULL; s = s->next) {
		fprintf(rsp, "%u", s->node->id);
		if (s->next != NULL) {
			fprintf(rsp, ",");
		}
	}
	fprintf(rsp, "]");
}

int query_node_ids(coordinates_t loc, node_select_t *sel, FILE *rsp)
{
	int count = 0;
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		if (loc.monitor != NULL && m != loc.monitor) {
			continue;
		}
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
			if (loc.desktop != NULL && d != loc.desktop) {
				continue;
			}
			count += query_node_ids_in(d->root, d, m, loc, sel, rsp);
		}
	}
	return count;
}

int query_node_ids_in(node_t *n, desktop_t *d, monitor_t *m, coordinates_t loc, node_select_t *sel, FILE *rsp)
{
	int count = 0;
	if (n == NULL) {
		return 0;
	} else {
		coordinates_t ref = {mon, mon->desk, mon->desk->focus};
		coordinates_t trg = {m, d, n};
		if ((loc.node == NULL || n == loc.node) &&
		    (sel == NULL || node_matches(&trg, &ref, *sel))) {
			fprintf(rsp, "0x%08X\n", n->id);
			count++;
		}
		count += query_node_ids_in(n->first_child, d, m, loc, sel, rsp);
		count += query_node_ids_in(n->second_child, d, m, loc, sel, rsp);
	}
	return count;
}

int query_desktop_ids(coordinates_t loc, desktop_select_t *sel, FILE *rsp)
{
	int count = 0;
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		if (loc.monitor != NULL && m != loc.monitor) {
			continue;
		}
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
			coordinates_t ref = {mon, mon->desk, NULL};
			coordinates_t trg = {m, d, NULL};
			if ((loc.desktop != NULL && d != loc.desktop) ||
			    (sel != NULL && !desktop_matches(&trg, &ref, *sel))) {
				continue;
			}
			fprintf(rsp, "0x%08X\n", d->id);
			count++;
		}
	}
	return count;
}

int query_monitor_ids(coordinates_t loc, monitor_select_t *sel, FILE *rsp)
{
	int count = 0;
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		coordinates_t ref = {mon, NULL, NULL};
		coordinates_t trg = {m, NULL, NULL};
		if ((loc.monitor != NULL && m != loc.monitor) ||
			(sel != NULL && !monitor_matches(&trg, &ref, *sel))) {
			continue;
		}
		fprintf(rsp, "0x%08X\n", m->id);
		count++;
	}
	return count;
}

void print_modifier_mask(uint16_t m, FILE *rsp)
{
	switch (m) {
		case XCB_MOD_MASK_SHIFT:
			fprintf(rsp, "shift");
			break;
		case XCB_MOD_MASK_CONTROL:
			fprintf(rsp, "control");
			break;
		case XCB_MOD_MASK_LOCK:
			fprintf(rsp, "lock");
			break;
		case XCB_MOD_MASK_1:
			fprintf(rsp, "mod1");
			break;
		case XCB_MOD_MASK_2:
			fprintf(rsp, "mod2");
			break;
		case XCB_MOD_MASK_3:
			fprintf(rsp, "mod3");
			break;
		case XCB_MOD_MASK_4:
			fprintf(rsp, "mod4");
			break;
		case XCB_MOD_MASK_5:
			fprintf(rsp, "mod5");
			break;
	}
}

void print_pointer_action(pointer_action_t a, FILE *rsp)
{
	switch (a) {
		case ACTION_FOCUS:
			fprintf(rsp, "focus");
			break;
		case ACTION_MOVE:
			fprintf(rsp, "move");
			break;
		case ACTION_RESIZE_SIDE:
			fprintf(rsp, "resize_side");
			break;
		case ACTION_RESIZE_CORNER:
			fprintf(rsp, "resize_corner");
			break;
	}
}

node_select_t make_node_select(void)
{
	node_select_t sel = {
		.automatic = OPTION_NONE,
		.focused = OPTION_NONE,
		.local = OPTION_NONE,
		.leaf = OPTION_NONE,
		.window = OPTION_NONE,
		.tiled = OPTION_NONE,
		.pseudo_tiled = OPTION_NONE,
		.floating = OPTION_NONE,
		.fullscreen = OPTION_NONE,
		.locked = OPTION_NONE,
		.sticky = OPTION_NONE,
		.private = OPTION_NONE,
		.urgent = OPTION_NONE,
		.same_class = OPTION_NONE,
		.below = OPTION_NONE,
		.normal = OPTION_NONE,
		.above = OPTION_NONE
	};
	return sel;
}

desktop_select_t make_desktop_select(void)
{
	desktop_select_t sel = {
		.occupied = OPTION_NONE,
		.focused = OPTION_NONE,
		.urgent = OPTION_NONE,
		.local = OPTION_NONE
	};
	return sel;
}

monitor_select_t make_monitor_select(void)
{
	monitor_select_t sel = {
		.occupied = OPTION_NONE,
		.focused = OPTION_NONE
	};
	return sel;
}

int node_from_desc(char *desc, coordinates_t *ref, coordinates_t *dst)
{
	char *desc_copy = copy_string(desc, strlen(desc));
	desc = desc_copy;
	node_select_t sel = make_node_select();
	char *colon = strrchr(desc, ':');

	if (!parse_node_modifiers(colon != NULL ? colon : desc, &sel)) {
		free(desc_copy);
		return SELECTOR_BAD_MODIFIERS;
	}

	dst->monitor = ref->monitor;
	dst->desktop = ref->desktop;
	dst->node = NULL;

	direction_t dir;
	cycle_dir_t cyc;
	history_dir_t hdi;
	if (parse_direction(desc, &dir)) {
		dst->node = nearest_neighbor(ref->monitor, ref->desktop, ref->node, dir, sel);
		if (dst->node == NULL && mon_head != mon_tail) {
			monitor_t *m = nearest_monitor(ref->monitor, dir, make_monitor_select());
			if (m != NULL) {
				coordinates_t loc = {m, m->desk, m->desk->focus};
				if (node_matches(&loc, ref, sel)) {
					dst->monitor = m;
					dst->desktop = m->desk;
					dst->node = m->desk->focus;
				}
			}
		}
	} else if (parse_cycle_direction(desc, &cyc)) {
		dst->node = closest_node(ref->monitor, ref->desktop, ref->node, cyc, sel);
	} else if (parse_history_direction(desc, &hdi)) {
		history_find_node(hdi, ref, dst, sel);
	} else if (streq("last", desc)) {
		history_find_node(HISTORY_OLDER, ref, dst, sel);
	} else if (streq("biggest", desc)) {
		dst->node = find_biggest(ref->monitor, ref->desktop, ref->node, sel);
	} else if (streq("focused", desc)) {
		coordinates_t loc = {mon, mon->desk, mon->desk->focus};
		if (node_matches(&loc, ref, sel)) {
			dst->monitor = mon;
			dst->desktop = mon->desk;
			dst->node = mon->desk->focus;
		}
	} else if (streq("pointed", desc)) {
		xcb_window_t win;
		query_pointer(&win, NULL);
		if (locate_window(win, dst) && node_matches(dst, ref, sel)) {
			return SELECTOR_OK;
		} else {
			return SELECTOR_INVALID;
		}
	} else if (*desc == '@') {
		desc++;
		if (colon != NULL) {
			*colon = '\0';
			int ret;
			if ((ret = desktop_from_desc(desc, ref, dst)) == SELECTOR_OK) {
				desc = colon + 1;
			} else {
				free(desc_copy);
				return ret;
			}
		}
		dst->node = (*desc == '/' ? dst->desktop->root : dst->desktop->focus);
		char *move = strtok(desc, PTH_TOK);
		while (move != NULL && dst->node != NULL) {
			if (streq("first", move) || streq("1", move)) {
				dst->node = dst->node->first_child;
			} else if (streq("second", move) || streq("2", move)) {
				dst->node = dst->node->second_child;
			} else if (streq("parent", move)) {
				dst->node = dst->node->parent;
			} else if (streq("brother", move)) {
				dst->node = brother_tree(dst->node);
			} else {
				direction_t dir;
				if (parse_direction(move, &dir)) {
					dst->node = find_fence(dst->node, dir);
				} else {
					free(desc_copy);
					return SELECTOR_BAD_DESCRIPTOR;
				}
			}
			move = strtok(NULL, PTH_TOK);
		}
		free(desc_copy);
		if (dst->node != NULL) {
			if (node_matches(dst, ref, sel)) {
				return SELECTOR_OK;
			} else {
				return SELECTOR_INVALID;
			}
		}
		return SELECTOR_OK;
	} else {
		uint32_t id;
		if (parse_id(desc, &id)) {
			free(desc_copy);
			if (find_by_id(id, dst) && node_matches(dst, ref, sel)) {
				return SELECTOR_OK;
			} else {
				return SELECTOR_INVALID;
			}
		} else {
			free(desc_copy);
			return SELECTOR_BAD_DESCRIPTOR;
		}
	}

	free(desc_copy);

	if (dst->node == NULL) {
		return SELECTOR_INVALID;
	}

	return SELECTOR_OK;
}

int desktop_from_desc(char *desc, coordinates_t *ref, coordinates_t *dst)
{
	char *desc_copy = copy_string(desc, strlen(desc));
	desc = desc_copy;
	desktop_select_t sel = make_desktop_select();
	char *colon = strrchr(desc, ':');

	if (!parse_desktop_modifiers(colon != NULL ? colon : desc, &sel)) {
		free(desc_copy);
		return SELECTOR_BAD_MODIFIERS;
	}

	dst->desktop = NULL;

	cycle_dir_t cyc;
	history_dir_t hdi;
	uint16_t idx;
	uint32_t id;
	if (parse_cycle_direction(desc, &cyc)) {
		dst->monitor = ref->monitor;
		dst->desktop = closest_desktop(ref->monitor, ref->desktop, cyc, sel);
	} else if (parse_history_direction(desc, &hdi)) {
		history_find_desktop(hdi, ref, dst, sel);
	} else if (streq("last", desc)) {
		history_find_desktop(HISTORY_OLDER, ref, dst, sel);
	} else if (streq("focused", desc)) {
		coordinates_t loc = {mon, mon->desk, NULL};
		if (desktop_matches(&loc, ref, sel)) {
			dst->monitor = mon;
			dst->desktop = mon->desk;
		}
	} else if (colon != NULL) {
		*colon = '\0';
		int ret;
		if ((ret = monitor_from_desc(desc, ref, dst)) == SELECTOR_OK) {
			if (streq("focused", colon + 1)) {
				coordinates_t loc = {dst->monitor, dst->monitor->desk, NULL};
				if (desktop_matches(&loc, ref, sel)) {
					dst->desktop = dst->monitor->desk;
				}
			} else if (parse_index(colon + 1, &idx)) {
				free(desc_copy);
				if (desktop_from_index(idx, dst, dst->monitor) && desktop_matches(dst, ref, sel)) {
					return SELECTOR_OK;
				} else {
					return SELECTOR_INVALID;
				}
			} else {
				free(desc_copy);
				return SELECTOR_BAD_DESCRIPTOR;
			}
		} else {
			free(desc_copy);
			return ret;
		}
	} else if (parse_index(desc, &idx) && desktop_from_index(idx, dst, NULL)) {
		free(desc_copy);
		if (desktop_matches(dst, ref, sel)) {
			return SELECTOR_OK;
		} else {
			return SELECTOR_INVALID;
		}
	} else if (parse_id(desc, &id) && desktop_from_id(id, dst, NULL)) {
		free(desc_copy);
		if (desktop_matches(dst, ref, sel)) {
			return SELECTOR_OK;
		} else {
			return SELECTOR_INVALID;
		}
	} else {
		if (locate_desktop(desc, dst)) {
			free(desc_copy);
			if (desktop_matches(dst, ref, sel)) {
				return SELECTOR_OK;
			} else {
				return SELECTOR_INVALID;
			}
		} else {
			free(desc_copy);
			return SELECTOR_BAD_DESCRIPTOR;
		}
	}

	free(desc_copy);

	if (dst->desktop == NULL) {
		return SELECTOR_INVALID;
	}

	return SELECTOR_OK;
}

int monitor_from_desc(char *desc, coordinates_t *ref, coordinates_t *dst)
{
	char *desc_copy = copy_string(desc, strlen(desc));
	desc = desc_copy;
	monitor_select_t sel = make_monitor_select();

	if (!parse_monitor_modifiers(desc, &sel)) {
		free(desc_copy);
		return SELECTOR_BAD_MODIFIERS;
	}

	dst->monitor = NULL;

	direction_t dir;
	cycle_dir_t cyc;
	history_dir_t hdi;
	uint16_t idx;
	uint32_t id;
	if (parse_direction(desc, &dir)) {
		dst->monitor = nearest_monitor(ref->monitor, dir, sel);
	} else if (parse_cycle_direction(desc, &cyc)) {
		dst->monitor = closest_monitor(ref->monitor, cyc, sel);
	} else if (parse_history_direction(desc, &hdi)) {
		history_find_monitor(hdi, ref, dst, sel);
	} else if (streq("last", desc)) {
		history_find_monitor(HISTORY_OLDER, ref, dst, sel);
	} else if (streq("primary", desc)) {
		if (pri_mon != NULL) {
			coordinates_t loc = {pri_mon, NULL, NULL};
			if (monitor_matches(&loc, ref, sel)) {
				dst->monitor = pri_mon;
			}
		}
	} else if (streq("focused", desc)) {
		coordinates_t loc = {mon, NULL, NULL};
		if (monitor_matches(&loc, ref, sel)) {
			dst->monitor = mon;
		}
	} else if (parse_index(desc, &idx) && monitor_from_index(idx, dst)) {
		free(desc_copy);
		if (monitor_matches(dst, ref, sel)) {
			return SELECTOR_OK;
		} else {
			return SELECTOR_INVALID;
		}
	} else if (parse_id(desc, &id) && monitor_from_id(id, dst)) {
		free(desc_copy);
		if (monitor_matches(dst, ref, sel)) {
			return SELECTOR_OK;
		} else {
			return SELECTOR_INVALID;
		}
	} else {
		if (locate_monitor(desc, dst)) {
			free(desc_copy);
			if (monitor_matches(dst, ref, sel)) {
				return SELECTOR_OK;
			} else {
				return SELECTOR_INVALID;
			}
		} else {
			free(desc_copy);
			return SELECTOR_BAD_DESCRIPTOR;
		}
	}

	free(desc_copy);

	if (dst->monitor == NULL) {
		return SELECTOR_INVALID;
	}

	return SELECTOR_OK;
}

bool locate_window(xcb_window_t win, coordinates_t *loc)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
			for (node_t *n = first_extrema(d->root); n != NULL; n = next_leaf(n, d->root)) {
				if (n->client == NULL) {
					continue;
				}
				if (n->id == win) {
					loc->monitor = m;
					loc->desktop = d;
					loc->node = n;
					return true;
				}
			}
		}
	}
	return false;
}

bool locate_desktop(char *name, coordinates_t *loc)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
			if (streq(d->name, name)) {
				loc->monitor = m;
				loc->desktop = d;
				return true;
			}
		}
	}
	return false;
}

bool locate_monitor(char *name, coordinates_t *loc)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		if (streq(m->name, name)) {
			loc->monitor = m;
			return true;
		}
	}
	return false;
}

bool desktop_from_id(uint32_t id, coordinates_t *loc, monitor_t *mm)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		if (mm != NULL && m != mm) {
			continue;
		}
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next) {
			if (d->id == id) {
				loc->monitor = m;
				loc->desktop = d;
				loc->node = NULL;
				return true;
			}
		}
	}
	return false;
}

bool desktop_from_index(uint16_t idx, coordinates_t *loc, monitor_t *mm)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		if (mm != NULL && m != mm) {
			continue;
		}
		for (desktop_t *d = m->desk_head; d != NULL; d = d->next, idx--) {
			if (idx == 1) {
				loc->monitor = m;
				loc->desktop = d;
				loc->node = NULL;
				return true;
			}
		}
	}
	return false;
}

bool monitor_from_id(uint32_t id, coordinates_t *loc)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next) {
		if (m->id == id) {
			loc->monitor = m;
			loc->desktop = NULL;
			loc->node = NULL;
			return true;
		}
	}
	return false;
}

bool monitor_from_index(int idx, coordinates_t *loc)
{
	for (monitor_t *m = mon_head; m != NULL; m = m->next, idx--) {
		if (idx == 1) {
			loc->monitor = m;
			loc->desktop = NULL;
			loc->node = NULL;
			return true;
		}
	}
	return false;
}

bool node_matches(coordinates_t *loc, coordinates_t *ref, node_select_t sel)
{
	if (loc->node == NULL) {
		return false;
	}

	if (sel.focused != OPTION_NONE &&
	    loc->node != mon->desk->focus
	    ? sel.focused == OPTION_TRUE
	    : sel.focused == OPTION_FALSE) {
		return false;
	}

	if (sel.automatic != OPTION_NONE &&
	    loc->node->presel != NULL
	    ? sel.automatic == OPTION_TRUE
	    : sel.automatic == OPTION_FALSE) {
		return false;
	}

	if (sel.local != OPTION_NONE &&
	    loc->desktop != ref->desktop
	    ? sel.local == OPTION_TRUE
	    : sel.local == OPTION_FALSE) {
		return false;
	}

	if (sel.leaf != OPTION_NONE &&
	    !is_leaf(loc->node)
	    ? sel.leaf == OPTION_TRUE
	    : sel.leaf == OPTION_FALSE) {
		return false;
	}

	if (sel.window != OPTION_NONE &&
	    loc->node->client == NULL
	    ? sel.window == OPTION_TRUE
	    : sel.window == OPTION_FALSE) {
		return false;
	}

#define NFLAG(p) \
	if (sel.p != OPTION_NONE && \
	    !loc->node->p \
	    ? sel.p == OPTION_TRUE \
	    : sel.p == OPTION_FALSE) { \
		return false; \
	}
	NFLAG(sticky)
	NFLAG(private)
	NFLAG(locked)
#undef WFLAG

	if (loc->node->client == NULL &&
		(sel.same_class != OPTION_NONE ||
		 sel.tiled != OPTION_NONE ||
		 sel.pseudo_tiled != OPTION_NONE ||
		 sel.floating != OPTION_NONE ||
		 sel.fullscreen != OPTION_NONE ||
		 sel.below != OPTION_NONE ||
		 sel.normal != OPTION_NONE ||
		 sel.above != OPTION_NONE ||
		 sel.urgent != OPTION_NONE)) {
		return false;
	}

	if (ref->node != NULL && ref->node->client != NULL &&
	    sel.same_class != OPTION_NONE &&
	    streq(loc->node->client->class_name, ref->node->client->class_name)
	    ? sel.same_class == OPTION_FALSE
	    : sel.same_class == OPTION_TRUE) {
		return false;
	}

#define WSTATE(p, e) \
	if (sel.p != OPTION_NONE && \
	    loc->node->client->state != e \
	    ? sel.p == OPTION_TRUE \
	    : sel.p == OPTION_FALSE) { \
		return false; \
	}
	WSTATE(tiled, STATE_TILED)
	WSTATE(pseudo_tiled, STATE_PSEUDO_TILED)
	WSTATE(floating, STATE_FLOATING)
	WSTATE(fullscreen, STATE_FULLSCREEN)
#undef WSTATE

#define WLAYER(p, e) \
	if (sel.p != OPTION_NONE && \
	    loc->node->client->layer != e \
	    ? sel.p == OPTION_TRUE \
	    : sel.p == OPTION_FALSE) { \
		return false; \
	}
	WLAYER(below, LAYER_BELOW)
	WLAYER(normal, LAYER_NORMAL)
	WLAYER(above, LAYER_ABOVE)
#undef WLAYER

#define WFLAG(p) \
	if (sel.p != OPTION_NONE && \
	    !loc->node->client->p \
	    ? sel.p == OPTION_TRUE \
	    : sel.p == OPTION_FALSE) { \
		return false; \
	}
	WFLAG(urgent)
#undef WFLAG

	return true;
}

bool desktop_matches(coordinates_t *loc, coordinates_t *ref, desktop_select_t sel)
{
	if (sel.occupied != OPTION_NONE &&
	    loc->desktop->root == NULL
	    ? sel.occupied == OPTION_TRUE
	    : sel.occupied == OPTION_FALSE) {
		return false;
	}

	if (sel.focused != OPTION_NONE &&
	    mon->desk != loc->desktop
	    ? sel.focused == OPTION_TRUE
	    : sel.focused == OPTION_FALSE) {
		return false;
	}

	if (sel.urgent != OPTION_NONE &&
	    !is_urgent(loc->desktop)
	    ? sel.urgent == OPTION_TRUE
	    : sel.urgent == OPTION_FALSE) {
		return false;
	}

	if (sel.local != OPTION_NONE &&
	    ref->monitor != loc->monitor
	    ? sel.local == OPTION_TRUE
	    : sel.local == OPTION_FALSE) {
		return false;
	}

	return true;
}

bool monitor_matches(coordinates_t *loc, __attribute__((unused)) coordinates_t *ref, monitor_select_t sel)
{
	if (sel.occupied != OPTION_NONE &&
	    loc->monitor->desk->root == NULL
	    ? sel.occupied == OPTION_TRUE
	    : sel.occupied == OPTION_FALSE) {
		return false;
	}

	if (sel.focused != OPTION_NONE &&
	    mon != loc->monitor
	    ? sel.focused == OPTION_TRUE
	    : sel.focused == OPTION_FALSE) {
		return false;
	}

	return true;
}
