/* $Id$ */

#include "stdafx.h"
#include "openttd.h"
#include "debug.h"
#include "functions.h"
#include "strings.h"
#include "table/strings.h"
#include "table/sprites.h"
#include "map.h"
#include "gui.h"
#include "window.h"
#include "gfx.h"
#include "command.h"
#include "viewport.h"
#include "industry.h"
#include "town.h"
#include "variables.h"

/* Present in table/build_industry.h" */
extern const byte _build_industry_types[4][12];
extern const byte _industry_type_costs[37];

static void UpdateIndustryProduction(Industry *i);

static void BuildIndustryWndProc(Window *w, WindowEvent *e)
{
	switch (e->event) {
	case WE_PAINT:
		DrawWindowWidgets(w);
		if (_thd.place_mode == 1 && _thd.window_class == WC_BUILD_INDUSTRY) {
			int ind_type = _build_industry_types[_opt_ptr->landscape][WP(w,def_d).data_1];

			SetDParam(0, (_price.build_industry >> 5) * GetIndustrySpec(ind_type)->cost_multiplier);
			DrawStringCentered(85, w->height - 21, STR_482F_COST, 0);
		}
		break;

	case WE_CLICK: {
		int wid = e->click.widget;
		if (wid >= 3) {
			if (HandlePlacePushButton(w, wid, SPR_CURSOR_INDUSTRY, 1, NULL))
				WP(w,def_d).data_1 = wid - 3;
		}
	} break;

	case WE_PLACE_OBJ:
		if (DoCommandP(e->place.tile, _build_industry_types[_opt_ptr->landscape][WP(w,def_d).data_1], 0, NULL, CMD_BUILD_INDUSTRY | CMD_MSG(STR_4830_CAN_T_CONSTRUCT_THIS_INDUSTRY)))
			ResetObjectToPlace();
		break;

	case WE_ABORT_PLACE_OBJ:
		w->click_state = 0;
		SetWindowDirty(w);
		break;
	}
}

static const Widget _build_industry_land0_widgets[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,											STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,		STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   115, 0x0,														STR_NULL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0241_POWER_STATION,				STR_0263_CONSTRUCT_POWER_STATION},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_0242_SAWMILL,							STR_0264_CONSTRUCT_SAWMILL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_0244_OIL_REFINERY,					STR_0266_CONSTRUCT_OIL_REFINERY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_0246_FACTORY,							STR_0268_CONSTRUCT_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68,    79, STR_0247_STEEL_MILL,						STR_0269_CONSTRUCT_STEEL_MILL},
{   WIDGETS_END},
};

static const Widget _build_industry_land1_widgets[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,											STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,		STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   115, 0x0,														STR_NULL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0241_POWER_STATION,				STR_0263_CONSTRUCT_POWER_STATION},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_024C_PAPER_MILL,						STR_026E_CONSTRUCT_PAPER_MILL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_0244_OIL_REFINERY,					STR_0266_CONSTRUCT_OIL_REFINERY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_024D_FOOD_PROCESSING_PLANT,STR_026F_CONSTRUCT_FOOD_PROCESSING},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68,    79, STR_024E_PRINTING_WORKS,				STR_0270_CONSTRUCT_PRINTING_WORKS},
{   WIDGETS_END},
};

static const Widget _build_industry_land2_widgets[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,											STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,		STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   115, 0x0,														STR_NULL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0250_LUMBER_MILL,					STR_0273_CONSTRUCT_LUMBER_MILL_TO},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_024D_FOOD_PROCESSING_PLANT,STR_026F_CONSTRUCT_FOOD_PROCESSING},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_0244_OIL_REFINERY,					STR_0266_CONSTRUCT_OIL_REFINERY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_0246_FACTORY,							STR_0268_CONSTRUCT_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68,    79, STR_0254_WATER_TOWER,					STR_0277_CONSTRUCT_WATER_TOWER_CAN},
{   WIDGETS_END},
};

static const Widget _build_industry_land3_widgets[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,											STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,		STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   115, 0x0,														STR_NULL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0258_CANDY_FACTORY,				STR_027B_CONSTRUCT_CANDY_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_025B_TOY_SHOP,							STR_027E_CONSTRUCT_TOY_SHOP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_025C_TOY_FACTORY,					STR_027F_CONSTRUCT_TOY_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_025E_FIZZY_DRINK_FACTORY,	STR_0281_CONSTRUCT_FIZZY_DRINK_FACTORY},
{   WIDGETS_END},
};

static const Widget _build_industry_land0_widgets_extra[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,							STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   187, 0x0,										STR_NULL},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0241_POWER_STATION,STR_0263_CONSTRUCT_POWER_STATION},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_0242_SAWMILL,			STR_0264_CONSTRUCT_SAWMILL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_0244_OIL_REFINERY,	STR_0266_CONSTRUCT_OIL_REFINERY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_0246_FACTORY,					STR_0268_CONSTRUCT_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68,    79, STR_0247_STEEL_MILL,		STR_0269_CONSTRUCT_STEEL_MILL},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    84,    95, STR_0240_COAL_MINE,		STR_CONSTRUCT_COAL_MINE_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    97,    108, STR_0243_FOREST,			STR_CONSTRUCT_FOREST_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    110,    121, STR_0245_OIL_RIG,		STR_CONSTRUCT_OIL_RIG_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    123,    134, STR_0248_FARM,						STR_CONSTRUCT_FARM_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    136,    147, STR_024A_OIL_WELLS,			STR_CONSTRUCT_OIL_WELLS_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    149,    160, STR_0249_IRON_ORE_MINE,	STR_CONSTRUCT_IRON_ORE_MINE_TIP},

{   WIDGETS_END},
};

static const Widget _build_industry_land1_widgets_extra[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,								STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,		STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   174, 0x0,											STR_NULL},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0241_POWER_STATION,	STR_0263_CONSTRUCT_POWER_STATION},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_024C_PAPER_MILL,			STR_026E_CONSTRUCT_PAPER_MILL},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_0244_OIL_REFINERY,		STR_0266_CONSTRUCT_OIL_REFINERY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_024D_FOOD_PROCESSING_PLANT,STR_026F_CONSTRUCT_FOOD_PROCESSING},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68,    79, STR_024E_PRINTING_WORKS,	STR_0270_CONSTRUCT_PRINTING_WORKS},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    81+3,    92+3, STR_0240_COAL_MINE,	STR_CONSTRUCT_COAL_MINE_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    94+3,   105+3, STR_0243_FOREST,			STR_CONSTRUCT_FOREST_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    107+3,  118+3, STR_0248_FARM,				STR_CONSTRUCT_FARM_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    120+3,  131+3, STR_024A_OIL_WELLS,	STR_CONSTRUCT_OIL_WELLS_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    133+3,  144+3, STR_024F_GOLD_MINE,	STR_CONSTRUCT_GOLD_MINE_TIP},
{   WIDGETS_END},
};

static const Widget _build_industry_land2_widgets_extra[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,							STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,			STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   200, 0x0,										STR_NULL},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0250_LUMBER_MILL,	STR_0273_CONSTRUCT_LUMBER_MILL_TO},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_024D_FOOD_PROCESSING_PLANT,STR_026F_CONSTRUCT_FOOD_PROCESSING},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_0244_OIL_REFINERY,	STR_0266_CONSTRUCT_OIL_REFINERY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_0246_FACTORY,			STR_0268_CONSTRUCT_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68,    79, STR_0254_WATER_TOWER,	STR_0277_CONSTRUCT_WATER_TOWER_CAN},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    81+3,    92+3, STR_024A_OIL_WELLS,STR_CONSTRUCT_OIL_WELLS_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    94+3,    105+3, STR_0255_DIAMOND_MINE,			STR_CONSTRUCT_DIAMOND_MINE_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    107+3,    118+3, STR_0256_COPPER_ORE_MINE,	STR_CONSTRUCT_COPPER_ORE_MINE_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    120+3,    131+3, STR_0248_FARM,		STR_CONSTRUCT_FARM_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    133+3,    144+3, STR_0251_FRUIT_PLANTATION,	STR_CONSTRUCT_FRUIT_PLANTATION_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    146+3,    157+3, STR_0252_RUBBER_PLANTATION,STR_CONSTRUCT_RUBBER_PLANTATION_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    159+3,    170+3, STR_0253_WATER_SUPPLY,			STR_CONSTRUCT_WATER_SUPPLY_TIP},
{   WIDGETS_END},
};

static const Widget _build_industry_land3_widgets_extra[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     7,     0,    10,     0,    13, STR_00C5,								STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     7,    11,   169,     0,    13, STR_0314_FUND_NEW_INDUSTRY,			STR_018C_WINDOW_TITLE_DRAG_THIS},
{      WWT_PANEL,   RESIZE_NONE,     7,     0,   169,    14,   187, 0x0,	STR_NULL},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    16,    27, STR_0258_CANDY_FACTORY,	STR_027B_CONSTRUCT_CANDY_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    29,    40, STR_025B_TOY_SHOP,				STR_027E_CONSTRUCT_TOY_SHOP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    42,    53, STR_025C_TOY_FACTORY,		STR_027F_CONSTRUCT_TOY_FACTORY},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    55,    66, STR_025E_FIZZY_DRINK_FACTORY,		STR_0281_CONSTRUCT_FIZZY_DRINK_FACTORY},

{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    68+3,    79+3, STR_0257_COTTON_CANDY_FOREST,STR_CONSTRUCT_COTTON_CANDY_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    81+3,    92+3, STR_0259_BATTERY_FARM,				STR_CONSTRUCT_BATTERY_FARM_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    94+3,    105+3, STR_025A_COLA_WELLS,				STR_CONSTRUCT_COLA_WELLS_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    107+3,    118+3, STR_025D_PLASTIC_FOUNTAINS,STR_CONSTRUCT_PLASTIC_FOUNTAINS_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    120+3,    131+3, STR_025F_BUBBLE_GENERATOR,	STR_CONSTRUCT_BUBBLE_GENERATOR_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    133+3,    144+3, STR_0260_TOFFEE_QUARRY,		STR_CONSTRUCT_TOFFEE_QUARRY_TIP},
{    WWT_TEXTBTN,   RESIZE_NONE,    14,     2,   167,    146+3,    157+3, STR_0261_SUGAR_MINE,				STR_CONSTRUCT_SUGAR_MINE_TIP},
{   WIDGETS_END},
};


static const WindowDesc _build_industry_land0_desc = {
	-1, -1, 170, 116,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land0_widgets,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land1_desc = {
	-1, -1, 170, 116,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land1_widgets,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land2_desc = {
	-1, -1, 170, 116,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land2_widgets,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land3_desc = {
	-1, -1, 170, 116,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land3_widgets,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land0_desc_extra = {
	-1, -1, 170, 188,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land0_widgets_extra,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land1_desc_extra = {
	-1, -1, 170, 175,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land1_widgets_extra,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land2_desc_extra = {
	-1, -1, 170, 201,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land2_widgets_extra,
	BuildIndustryWndProc
};

static const WindowDesc _build_industry_land3_desc_extra = {
	-1, -1, 170, 188,
	WC_BUILD_INDUSTRY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET,
	_build_industry_land3_widgets_extra,
	BuildIndustryWndProc
};

static const WindowDesc * const _industry_window_desc[2][4] = {
	{
	&_build_industry_land0_desc,
	&_build_industry_land1_desc,
	&_build_industry_land2_desc,
	&_build_industry_land3_desc,
	},
	{
	&_build_industry_land0_desc_extra,
	&_build_industry_land1_desc_extra,
	&_build_industry_land2_desc_extra,
	&_build_industry_land3_desc_extra,
	},
};

void ShowBuildIndustryWindow(void)
{
	if (_current_player == OWNER_SPECTATOR) return;
	AllocateWindowDescFront(_industry_window_desc[_patches.build_rawmaterial_ind][_opt_ptr->landscape],0);
}

#define NEED_ALTERB	((_game_mode == GM_EDITOR || _cheats.setup_prod.value) && (i->accepts_cargo[0] == CT_INVALID || i->accepts_cargo[0] == CT_VALUABLES))
static void IndustryViewWndProc(Window *w, WindowEvent *e)
{
	// WP(w,vp2_d).data_1 is for the editbox line
	// WP(w,vp2_d).data_2 is for the clickline
	// WP(w,vp2_d).data_3 is for the click pos (left or right)

	switch (e->event) {
	case WE_PAINT: {
		const Industry *i = GetIndustry(w->window_number);

		SetDParam(0, w->window_number);
		DrawWindowWidgets(w);

		if (i->accepts_cargo[0] != CT_INVALID) {
			StringID str;

			SetDParam(0, _cargoc.names_s[i->accepts_cargo[0]]);
			str = STR_4827_REQUIRES;
			if (i->accepts_cargo[1] != CT_INVALID) {
				SetDParam(1, _cargoc.names_s[i->accepts_cargo[1]]);
				str = STR_4828_REQUIRES;
				if (i->accepts_cargo[2] != CT_INVALID) {
					SetDParam(2, _cargoc.names_s[i->accepts_cargo[2]]);
					str = STR_4829_REQUIRES;
				}
			}
			DrawString(2, 107, str, 0);
		}

		if (i->produced_cargo[0] != CT_INVALID) {
			DrawString(2, 117, STR_482A_PRODUCTION_LAST_MONTH, 0);

			SetDParam(0, _cargoc.names_long[i->produced_cargo[0]]);
			SetDParam(1, i->total_production[0]);

			SetDParam(2, i->pct_transported[0] * 100 >> 8);
			DrawString(4 + (NEED_ALTERB ? 30 : 0), 127, STR_482B_TRANSPORTED, 0);
			// Let's put out those buttons..
			if (NEED_ALTERB)
				DrawArrowButtons(5, 127, 3, (WP(w,vp2_d).data_2 == 1) ? WP(w,vp2_d).data_3 : 0, true);

			if (i->produced_cargo[1] != CT_INVALID) {
				SetDParam(0, _cargoc.names_long[i->produced_cargo[1]]);
				SetDParam(1, i->total_production[1]);
				SetDParam(2, i->pct_transported[1] * 100 >> 8);
				DrawString(4 + (NEED_ALTERB ? 30 : 0), 137, STR_482B_TRANSPORTED, 0);
				// Let's put out those buttons..
				if (NEED_ALTERB) {
					DrawArrowButtons(5, 137, 3, (WP(w,vp2_d).data_2 == 2) ? WP(w,vp2_d).data_3 : 0, true);
				}
			}
		}

		DrawWindowViewport(w);
		}
		break;

	case WE_CLICK: {
		Industry *i;

		switch (e->click.widget) {
		case 5: {
			int line, x;

			i = GetIndustry(w->window_number);

			// We should work if needed..
			if (!NEED_ALTERB) return;

			x = e->click.pt.x;
			line = (e->click.pt.y - 127) / 10;
			if (e->click.pt.y >= 127 && IS_INT_INSIDE(line, 0, 2) && i->produced_cargo[line] != CT_INVALID) {
				if (IS_INT_INSIDE(x, 5, 25) ) {
					/* Clicked buttons, decrease or increase production */
					if (x < 15) {
						i->production_rate[line] = maxu(i->production_rate[line] / 2, 1);
					} else {
						i->production_rate[line] = minu(i->production_rate[line] * 2, 255);
					}

					UpdateIndustryProduction(i);
					SetWindowDirty(w);
					w->flags4 |= 5 << WF_TIMEOUT_SHL;
					WP(w,vp2_d).data_2 = line+1;
					WP(w,vp2_d).data_3 = (x < 15 ? 1 : 2);
				} else if (IS_INT_INSIDE(x, 34, 160)) {
					// clicked the text
					WP(w,vp2_d).data_1 = line;
					SetDParam(0, i->production_rate[line] * 8);
					ShowQueryString(STR_CONFIG_PATCHES_INT32,
							STR_CONFIG_GAME_PRODUCTION,
							10, 100, w->window_class,
							w->window_number);
				}
			}
		} break;
		case 6:
			i = GetIndustry(w->window_number);
			ScrollMainWindowToTile(i->xy + TileDiffXY(1, 1));
		}	break;

		}
		break;
	case WE_TIMEOUT:
		WP(w,vp2_d).data_2 = 0;
		WP(w,vp2_d).data_3 = 0;
		SetWindowDirty(w);
		break;

	case WE_ON_EDIT_TEXT:
		if (e->edittext.str[0] != '\0') {
			Industry* i = GetIndustry(w->window_number);
			int line = WP(w,vp2_d).data_1;

			i->production_rate[line] = clampu(atoi(e->edittext.str), 0, 255);
			UpdateIndustryProduction(i);
			SetWindowDirty(w);
		}
	}
}

static void UpdateIndustryProduction(Industry *i)
{
	if (i->produced_cargo[0] != CT_INVALID)
		i->total_production[0] = 8 * i->production_rate[0];

	if (i->produced_cargo[1] != CT_INVALID)
		i->total_production[1] = 8 * i->production_rate[1];
}

static const Widget _industry_view_widgets[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,     9,     0,    10,     0,    13, STR_00C5,	STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,     9,    11,   247,     0,    13, STR_4801,	STR_018C_WINDOW_TITLE_DRAG_THIS},
{  WWT_STICKYBOX,   RESIZE_NONE,     9,   248,   259,     0,    13, 0x0,       STR_STICKY_BUTTON},
{     WWT_IMGBTN,   RESIZE_NONE,     9,     0,   259,    14,   105, 0x0,				STR_NULL},
{	  WWT_6,   RESIZE_NONE,     9,     2,   257,    16,   103, 0x0,				STR_NULL},
{     WWT_IMGBTN,   RESIZE_NONE,     9,     0,   259,   106,   147, 0x0,				STR_NULL},
{ WWT_PUSHTXTBTN,   RESIZE_NONE,     9,     0,   129,   148,   159, STR_00E4_LOCATION,	STR_482C_CENTER_THE_MAIN_VIEW_ON},
{     WWT_IMGBTN,   RESIZE_NONE,     9,   130,   259,   148,   159, 0x0,				STR_NULL},
{   WIDGETS_END},
};

static const WindowDesc _industry_view_desc = {
	-1, -1, 260, 160,
	WC_INDUSTRY_VIEW,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET | WDF_UNCLICK_BUTTONS | WDF_STICKY_BUTTON,
	_industry_view_widgets,
	IndustryViewWndProc
};

void ShowIndustryViewWindow(int industry)
{
	Window *w = AllocateWindowDescFront(&_industry_view_desc, industry);

	if (w != NULL) {
		w->flags4 |= WF_DISABLE_VP_SCROLL;
		WP(w,vp2_d).data_1 = 0;
		WP(w,vp2_d).data_2 = 0;
		WP(w,vp2_d).data_3 = 0;
		AssignWindowViewport(w, 3, 17, 0xFE, 0x56, GetIndustry(w->window_number)->xy + TileDiffXY(1, 1), 1);
	}
}

static const Widget _industry_directory_widgets[] = {
{   WWT_CLOSEBOX,   RESIZE_NONE,    13,     0,    10,     0,    13, STR_00C5,									STR_018B_CLOSE_WINDOW},
{    WWT_CAPTION,   RESIZE_NONE,    13,    11,   495,     0,    13, STR_INDUSTRYDIR_CAPTION,	STR_018C_WINDOW_TITLE_DRAG_THIS},
{  WWT_STICKYBOX,   RESIZE_NONE,    13,   496,   507,     0,    13, 0x0,											STR_STICKY_BUTTON},
{ WWT_PUSHTXTBTN,   RESIZE_NONE,    13,     0,   100,    14,    25, STR_SORT_BY_NAME,					STR_SORT_ORDER_TIP},
{ WWT_PUSHTXTBTN,   RESIZE_NONE,    13,   101,   200,    14,    25, STR_SORT_BY_TYPE,					STR_SORT_ORDER_TIP},
{ WWT_PUSHTXTBTN,   RESIZE_NONE,    13,   201,   300,    14,    25, STR_SORT_BY_PRODUCTION,		STR_SORT_ORDER_TIP},
{ WWT_PUSHTXTBTN,   RESIZE_NONE,    13,   301,   400,    14,    25, STR_SORT_BY_TRANSPORTED,	STR_SORT_ORDER_TIP},
{      WWT_PANEL,   RESIZE_NONE,    13,   401,   495,    14,    25, 0x0,											STR_NULL},
{     WWT_IMGBTN, RESIZE_BOTTOM,    13,     0,   495,    26,   189, 0x0,											STR_200A_TOWN_NAMES_CLICK_ON_NAME},
{  WWT_SCROLLBAR, RESIZE_BOTTOM,    13,   496,   507,    14,   177, 0x0,											STR_0190_SCROLL_BAR_SCROLLS_LIST},
{  WWT_RESIZEBOX,     RESIZE_TB,    13,   496,   507,   178,   189, 0x0,											STR_RESIZE_BUTTON},
{   WIDGETS_END},
};

static uint _num_industry_sort;

static char _bufcache[96];
static uint16 _last_industry_idx;

static byte _industry_sort_order;

static int CDECL GeneralIndustrySorter(const void *a, const void *b)
{
	char buf1[96];
	uint16 val;
	Industry *i = GetIndustry(*(const uint16*)a);
	Industry *j = GetIndustry(*(const uint16*)b);
	int r = 0;

	switch (_industry_sort_order >> 1) {
	/* case 0: Sort by Name (handled later) */
	case 1: /* Sort by Type */
		r = i->type - j->type;
		break;
	// FIXME - Production & Transported sort need to be inversed...but, WTF it does not wanna!
	// FIXME - And no simple --> "if (!(_industry_sort_order & 1)) r = -r;" hack at the bottom!!
	case 2: { /* Sort by Production */
		if (i->produced_cargo[0] != CT_INVALID && j->produced_cargo[0] != CT_INVALID) { // both industries produce cargo?
				if (i->produced_cargo[1] == CT_INVALID) // producing one or two things?
					r = j->total_production[0] - i->total_production[0];
				else
					r = (j->total_production[0] + j->total_production[1]) / 2 - (i->total_production[0] + i->total_production[1]) / 2;
		} else if (i->produced_cargo[0] == CT_INVALID && j->produced_cargo[0] == CT_INVALID) // none of them producing anything, let them go to the name-sorting
			r = 0;
		else if (i->produced_cargo[0] == CT_INVALID) // end up the non-producer industry first/last in list
			r = 1;
		else
			r = -1;
		break;
	}
	case 3: /* Sort by Transported amount */
		if (i->produced_cargo[0] != CT_INVALID && j->produced_cargo[0] != CT_INVALID) { // both industries produce cargo?
				if (i->produced_cargo[1] == CT_INVALID) // producing one or two things?
					r = (j->pct_transported[0] * 100 >> 8) - (i->pct_transported[0] * 100 >> 8);
				else
					r = ((j->pct_transported[0] * 100 >> 8) + (j->pct_transported[1] * 100 >> 8)) / 2 - ((i->pct_transported[0] * 100 >> 8) + (i->pct_transported[1] * 100 >> 8)) / 2;
		} else if (i->produced_cargo[0] == CT_INVALID && j->produced_cargo[0] == CT_INVALID) // none of them producing anything, let them go to the name-sorting
			r = 0;
		else if (i->produced_cargo[0] == CT_INVALID) // end up the non-producer industry first/last in list
			r = 1;
		else
			r = -1;
		break;
	}

	// default to string sorting if they are otherwise equal
	if (r == 0) {
		SetDParam(0, i->town->index);
		GetString(buf1, STR_TOWN);

		if ( (val=*(const uint16*)b) != _last_industry_idx) {
			_last_industry_idx = val;
			SetDParam(0, j->town->index);
			GetString(_bufcache, STR_TOWN);
		}
		r = strcmp(buf1, _bufcache);
	}

	if (_industry_sort_order & 1) r = -r;
	return r;
}

static void MakeSortedIndustryList(void)
{
	Industry *i;
	int n = 0;

	/* Create array for sorting */
	_industry_sort = realloc(_industry_sort, GetIndustryPoolSize() * sizeof(_industry_sort[0]));
	if (_industry_sort == NULL)
		error("Could not allocate memory for the industry-sorting-list");

	FOR_ALL_INDUSTRIES(i) {
		if (i->xy != 0) _industry_sort[n++] = i->index;
	}
	_num_industry_sort = n;
	_last_industry_idx = 0xFFFF; // used for "cache"

	qsort(_industry_sort, n, sizeof(_industry_sort[0]), GeneralIndustrySorter);

	DEBUG(misc, 1) ("Resorting Industries list...");
}


static void IndustryDirectoryWndProc(Window *w, WindowEvent *e)
{
	switch (e->event) {
	case WE_PAINT: {
		int n;
		uint p;
		static const uint16 _indicator_positions[4] = {88, 187, 284, 387};

		if (_industry_sort_dirty) {
			_industry_sort_dirty = false;
			MakeSortedIndustryList();
		}

		SetVScrollCount(w, _num_industry_sort);

		DrawWindowWidgets(w);
		DoDrawString(_industry_sort_order & 1 ? DOWNARROW : UPARROW, _indicator_positions[_industry_sort_order>>1], 15, 0x10);

		p = w->vscroll.pos;
		n = 0;

		while (p < _num_industry_sort) {
			const Industry *i = GetIndustry(_industry_sort[p]);

			SetDParam(0, i->index);
			if (i->produced_cargo[0] != CT_INVALID) {
				SetDParam(1, _cargoc.names_long[i->produced_cargo[0]]);
				SetDParam(2, i->total_production[0]);

				if (i->produced_cargo[1] != CT_INVALID) {
					SetDParam(3, _cargoc.names_long[i->produced_cargo[1]]);
					SetDParam(4, i->total_production[1]);
					SetDParam(5, i->pct_transported[0] * 100 >> 8);
					SetDParam(6, i->pct_transported[1] * 100 >> 8);
					DrawString(4, 28+n*10, STR_INDUSTRYDIR_ITEM_TWO, 0);
				} else {
					SetDParam(3, i->pct_transported[0] * 100 >> 8);
					DrawString(4, 28+n*10, STR_INDUSTRYDIR_ITEM, 0);
				}
			} else {
				DrawString(4, 28+n*10, STR_INDUSTRYDIR_ITEM_NOPROD, 0);
			}
			p++;
			if (++n == w->vscroll.cap) break;
		}
	} break;

	case WE_CLICK:
		switch (e->click.widget) {
		case 3: {
			_industry_sort_order = _industry_sort_order==0 ? 1 : 0;
			_industry_sort_dirty = true;
			SetWindowDirty(w);
		} break;

		case 4: {
			_industry_sort_order = _industry_sort_order==2 ? 3 : 2;
			_industry_sort_dirty = true;
			SetWindowDirty(w);
		} break;

		case 5: {
			_industry_sort_order = _industry_sort_order==4 ? 5 : 4;
			_industry_sort_dirty = true;
			SetWindowDirty(w);
		} break;

		case 6: {
			_industry_sort_order = _industry_sort_order==6 ? 7 : 6;
			_industry_sort_dirty = true;
			SetWindowDirty(w);
		} break;

		case 8: {
			int y = (e->click.pt.y - 28) / 10;
			uint16 p;

			if (!IS_INT_INSIDE(y, 0, w->vscroll.cap)) return;
			p = y + w->vscroll.pos;
			if (p < _num_industry_sort) {
				ScrollMainWindowToTile(GetIndustry(_industry_sort[p])->xy);
			}
		} break;
		}
		break;

	case WE_4:
		SetWindowDirty(w);
		break;

	case WE_RESIZE:
		w->vscroll.cap += e->sizing.diff.y / 10;
		break;
	}
}


/* Industry List */
static const WindowDesc _industry_directory_desc = {
	-1, -1, 508, 190,
	WC_INDUSTRY_DIRECTORY,0,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET | WDF_UNCLICK_BUTTONS | WDF_STICKY_BUTTON | WDF_RESIZABLE,
	_industry_directory_widgets,
	IndustryDirectoryWndProc
};


void ShowIndustryDirectory(void)
{
	Window *w = AllocateWindowDescFront(&_industry_directory_desc, 0);

	if (w != NULL) {
		w->vscroll.cap = 16;
		w->resize.height = w->height - 6 * 10; // minimum 10 items
		w->resize.step_height = 10;
		SetWindowDirty(w);
	}
}
