/*
* unit_flow_cfg.c
*
*  Created on: 2018Äê1ÔÂ19ÈÕ
*      Author: pli
*/



#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_flowstep.h"
#include "t_data_obj_test.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_flow_cfg.h"
#include "dev_log_sp.h"

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

static const uint8_t fileID_Default = 0x09;



static uint8_t fileId1_0									__attribute__ ((section (".configbuf_Flow")));
uint8_t preSubSteps[e_max_presub][PRESUB_STEP_MAX]  		__attribute__ ((section (".configbuf_Flow")));
uint8_t subSteps[e_max_substep][SUB_STEP_MAX]				__attribute__ ((section (".configbuf_Flow")));
uint8_t preMainSteps[e_max_premain][PRE_MAIN_STEP_MAX]		__attribute__ ((section (".configbuf_Flow")));

static uint8_t fileId2_0									__attribute__ ((section (".configbuf_Flow")));

static uint8_t fileId1_1									__attribute__ ((section (".configbuf_Flow")));
PumpCfg pumpCfg[e_max_pump]									__attribute__ ((section (".configbuf_Flow")));
uint8_t mainSteps[e_max_mainstep][MAIN_STEP_MAX]  			__attribute__ ((section (".configbuf_Flow")));
uint16_t timeDelay[e_max_timedelay]							__attribute__ ((section (".configbuf_Flow")));
SPAction spAction[e_max_sp]									__attribute__ ((section (".configbuf_Flow")));
static uint8_t fileId2_1									__attribute__ ((section (".configbuf_Flow")));


static uint8_t cmdValveDelay[e_cmd_id_max] =
{
    20, /* e_open_cera_v */ 
    5, /* e_open_air_v */ 
    5, /* e_open_samp_v */ 
    15, /* e_open_waste_v */ 
    5, /* e_open_dichro_v */ 
    5, /* e_open_blank_v */ 
    5, /* e_open_H2SO4_v */ 
    5, /* e_open_HgSO4_v */ 
    5, /* e_open_std_v */ 
    5, /* e_open_safety_v */ 
    5, /* e_open_lock */ 
    20, /* e_close_cera_v */ 
    5, /* e_close_air_v */ 
    5, /* e_close_samp_v */ 
    15, /* e_close_waste_v */ 
    5, /* e_close_dichro_v */ 
    5, /* e_close_blank_v */ 
    5, /* e_close_H2SO4_v */ 
    5, /* e_close_HgSO4_v */ 
    5, /* e_close_std_v */ 
    5, /* e_close_safety_v */ 
    5, /* e_close_lock */ 
    5, /* e_open_relay1 */ 
    5, /* e_open_relay2 */ 
    5, /* e_close_relay1 */ 
    5, /* e_close_relay2 */ 
    5, /* e_open_dig_fan_v */ 
    5, /* e_close_dig_fan_v */ 
    5, /*HPP, e_open_PCBA_fan_v */ 
    5, /*HPP, e_close_PCBA_fan_v */ 
    5, /*HPP, e_open_enclosure_fan_v */ 
    5, /*HPP, e_close_enclosure_fan_v */ 
    5, /* rev */ 
    5, /* rev */ 
    5, /* rev */ 
    5, /* rev */ 
    5, /* rev */ 
    5, /* rev-for space */ 
    5, /* deviceReset */ 
};


static const uint8_t preSubSteps_Default[e_max_presub][PRESUB_STEP_MAX] =
{
{/*120*/ 2, 52, 13, 6, 41, 17, 10, 1, 55, 12, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*121*/ 2, 52, 13, 9, 42, 20, 10, 1, 56, 12, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*122*/ 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*123*/ 2, 52, 13, 10, 1, 48, 12, 21, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*124*/ 2, 52, 13, 10, 1, 49, 12, 21, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*125*/ 2, 52, 13, 3, 45, 14, 10, 1, 59, 12, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*126*/ 2, 52, 13, 5, 43, 82, 16, 52, 82, 63, 5, 43, 108, 85, 72, 85, 16, 10, 1, 57, 12, 21, },
{/*127*/ 2, 52, 13, 5, 43, 82, 16, 52, 82, 63, 5, 43, 108, 85, 73, 85, 16, 10, 1, 57, 12, 21, },
{/*128*/ 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 28, 30, 32, 13, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*129*/ 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*130*/ 2, 71, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*131*/ 60, 47, 60, 47, 60, 47, 60, 47, 60, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*132*/ 60, 47, 60, 47, 60, 47, 60, 47, 60, 47, 60, 47, 60, 47, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*133*/ 2, 52, 13, 7, 40, 18, 82, 82, 82, 82, 82, 7, 40, 82, 109, 18, 10, 1, 54, 12, 21, 0, 0, 0, },
{/*134*/ 2, 52, 13, 8, 44, 19, 82, 82, 8, 44, 82, 109, 19, 10, 1, 58, 12, 21, 0, 0, 0, },
{/*135*/ 2, 52, 13, 6, 41, 17, 82, 82, 6, 41, 82, 109, 17, 10, 1, 55, 12, 21, 0, 0, 0, },
{/*136*/ 2, 52, 13, 9, 42, 20, 82, 82, 9, 42, 82, 109, 20, 10, 1, 56, 12, 21, 0, 0, 0, },
{/*137*/ 2, 52, 13, 3, 45, 14, 82, 82, 3, 45, 82, 109, 14, 10, 1, 59, 12, 21, 0, 0, 0, },
{/*138*/ 2, 52, 13, 5, 43, 16, 82, 82, 5, 43, 82, 109, 16, 10, 1, 57, 12, 21, 0, 0, 0, },
{/*139*/ 2, 52, 13, 5, 43, 82, 16, 82, 82, 5, 43, 82, 109, 16, 10, 1, 57, 12, 21, 0, 0, 0, },

};

//2	52	13	7	40	109	109	18	10	1	54	12	21	0	0	0	0	0	0	0	0


static const uint8_t subSteps_Default[e_max_substep][SUB_STEP_MAX] =
{
{/*140*/ 130, 133, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*141*/ 130, 134, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*142*/ 130, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*143*/ 130, 136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*144*/ 130, 137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*145*/ 130, 138, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*146*/ 130, 2, 53, 13, 10, 1, 64, 12, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*147*/ 130, 2, 53, 13, 10, 1, 62, 12, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*148*/ 130, 120, 120, 120, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*149*/ 130, 121, 121, 121, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*150*/ 130, 122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*151*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*152*/ 130, 2, 52, 13, 6, 47, 17, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*153*/ 130, 2, 52, 13, 9, 47, 20, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, },
{/*154*/ 130, 2, 52, 13, 3, 47, 14, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, },
{/*155*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*156*/ 130, 2, 47, 46, 13, 7, 64, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*157*/ 130, 2, 46, 13, 6, 64, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*158*/ 130, 2, 46, 13, 9, 64, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*159*/ 130, 2, 46, 13, 8, 64, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*160*/ 130, 2, 46, 13, 3, 64, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*161*/ 130, 2, 46, 13, 5, 64, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*162*/ 130, 6, 41, 17, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*163*/ 130, 133, 2, 52, 13, 7, 40, 18, 4, 10, 1, 131, 54, 12, 0, 0, 0, 0, 0, },
{/*164*/ 2, 52, 13, 10, 1, 49, 12, 21, 3, 132, 64, 99, 14, 0, 0, 0, 0, 0, 0, },
{/*165*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*166*/ 130, 123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*167*/ 130, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*168*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*169*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*170*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*171*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*172*/ 130, 2, 53, 13, 6, 64, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*173*/ 130, 2, 53, 13, 9, 64, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*174*/ 130, 9, 42, 20, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*175*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*176*/ 130, 3, 45, 14, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*177*/ 130, 125, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*178*/ 130, 2, 53, 13, 3, 64, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*179*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*180*/ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*181*/ 0, 0, 0, 0, 0, },
{/*182*/ 130, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*183*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*184*/ 130, 137, 137, 137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*185*/ 130, 133, 133, 133, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*186*/ 130, 134, 134, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*187*/ 130, 138, 138, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*188*/ 130, 135, 135, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*189*/ 130, 136, 136, 136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*190*/ 130, 123, 123, 123, 123, 123, 123, 123, 0, 0, 0, 0, 0, },
{/*191*/ 130, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*192*/ 130, 124, 124, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*193*/ 130, 122, 122, 122, 122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*194*/ 130, 3, 45, 14, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*195*/ 130, 8, 44, 19, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*196*/ 130, 5, 43, 16, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*197*/ 130, 7, 40, 18, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*198*/ 130, 9, 42, 20, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*199*/ 130, 6, 41, 17, 2, 53, 13, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*200 blank drain*/ 	130, 2, 53, 13, 6, 53, 17, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, },
{/*201 std1 drain*/ 	130, 2, 53, 13, 9, 53, 20, 4, 64, 15, 0, 0, 0, 0, 0, 0, },
{/*202 sample drain*/ 	130, 2, 53, 13, 3, 53, 14, 4, 64, 15, 0, 0, 0, 0, 0, 0, 0, },
{/*203*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*204*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*205*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*206*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*207*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*208*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{/*209*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

//102;start sample

//sample 184, back 178;     184, 178, 178,
//Cr6 - 145, back: 161;     145, 161,
//hg - 141, back 159;       141, 159
//so4 - 140, back 156;      140, 140, 156,



//mix 146
//heat to 50: 94
//heat to dig: 96

static const uint8_t preMainSteps_Default[e_max_premain][PRE_MAIN_STEP_MAX] =
{
{/*210*/ 140, 140, 156, 146, 94, 141, 159, 146, 145, 161, 146, 97, 188, 146, 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*211*/ 140, 140, 156, 146, 94, 141, 159, 146, 145, 161, 146, 97, 189, 146, 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*212*/ 180, 130, 98, 122, 123, 123, 84, 174, 174, 174, 149, 190, 102, 189, 173, 173, 141, 159, 145, 147, 161, 150, 38, 140, 140, 156, 146, 38, 88, 101, 95, 81, 100,  98, 190, 0, 0, 0, 0, 0, 0, 0},
{/*213*/ 180, 130, 98, 122, 123, 123, 84, 176, 176, 176, 191, 190, 102, 184, 178, 178, 141, 159, 145, 147, 161, 150, 94, 140, 140, 156, 146, 96, 88, 101, 95, 81, 100,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*214*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*215*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*216*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*217*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*218*/ 180, 130, 98, 122, 123, 123, 84, 174, 174, 174, 149, 190, 102,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*219 std1 pre*/ 180, 130, 98, 122, 123, 123, 84, 174, 174, 174, 149, 190, 102,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*220 std0 pre*/ 180, 130, 98, 122, 123, 123, 84, 162, 162, 162, 148, 190, 102,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*221 std1 */ 189, 146, 201, 158, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*222 std0 */ 188, 146, 200, 157, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*223 flush*/ 184, 124, 185, 124, 186, 192, 187, 124, 188, 189, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*224 sample*/ 184, 146, 202, 178, 178, 0},
{/*225 Cr6*/ 145, 146, 181, 161, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*226 hg*/ 141, 146, 181, 159, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*227 so4*/ 140, 146, 140, 146, 181, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{/*228 pre*/ 180, 130, 98, 122, 123, 123, 84, 176, 176, 176, 191, 190, 99, 0, 0, 0},
{/*229 end*/ 96, 88, 101, 95, 81, 100, 98, 190, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

};

//94;5500
//95:8500
static const uint8_t mainSteps_Default[e_max_mainstep][MAIN_STEP_MAX] =
{
		/*230*/{220, 227, 94, 226, 225, 97, 222, 146, 147, 229, \
				220, 227, 94, 226, 225, 97, 222, 146, 147, 229, \
				219, 227, 94, 226, 225, 97, 221, 146, 147, 229,},
		/*231*/{223, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		/*232*/{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		/*233*/{228, 227, 94, 226, 225, 97, 184, 146, 147, 229, 80, 98, 0, },
		/*234*/{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		/*235*/{220, 227, 94, 226, 225, 97, 222, 146, 147, 229, 80, 98, 0, },
		/*236*/{219, 227, 94, 226, 225, 97, 221, 146, 147, 229, 80, 98, 0, },
		/*237*/{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		/*238*/{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		/*239*/{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },


};

static const PumpCfg pumpCfg_Default[e_max_pump] =
{
{/*40*/ PULL|LED0_POS	,	20 	,	MAX_STEPS_PUMP0	,	0	,	0	,	FAILED	},
{/*41*/	PULL|LED0_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FAILED	},
{/*42*/	PULL|LED0_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FAILED	},
{/*43*/	PULL|LED0_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FAILED	},
{/*44*/	PULL|LED0_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FAILED	},
{/*45*/	PULL|LED0_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FAILED	},
{/*46*/	PULL|FREE_POS	,	1	,	AIR_STEPS_PUMP2	,	0	,	0	,	FAILED	},
{/*47*/	PULL|FREE_POS	,	2	,	AIR_STEPS_PUMP1	,	0	,	0	,	FIN_STEP	},
{/*48*/	PULL|LED0_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*49*/	PULL|LED1_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*50*/	PULL|LED1_POS	,	4	,	MAX_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*51*/	ACT_NONE	,	1	,	10	,	0	,	0	,	LOG_FLASH	},
{/*52*/	PULL|FREE_POS	,	1	,	AIR_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*53*/	PULL|END_POS	,	1	,	MAX_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*54*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*55*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*56*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*57*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*58*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*59*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*60*/	PUSH|FREE_POS	,	1	,	AIR_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*61*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*62*/	PUSH|END_POS	,	4	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*63*/	PUSH|FREE_POS	,	1	,	AIR_STEPS_PUMP0	,	0	,	0	,	LOG_FLASH	},
{/*64*/	PUSH|END_POS	,	1	,	PUSH_MAX_STEPS	,	0	,	0	,	FIN_STEP	},
{/*65*/	ACT_NONE	,	1	,	10	,	0	,	0	,	FIN_STEP	},
{/*66*/	ACT_NONE	,	1	,	10	,	0	,	0	,	FIN_STEP	},
{/*67*/	ACT_NONE	,	1	,	10	,	0	,	0	,	FIN_STEP	},
{/*68*/	ACT_NONE	,	1	,	10	,	0	,	0	,	FIN_STEP	},
{/*69*/	ACT_NONE	,	1	,	10	,	0	,	0	,	FIN_STEP	},
{/*70*/	ACT_NONE	,	1	,	10	,	0	,	0	,	LOG_FLASH	},
{/*71*/	PUSH|END_POS	,	1	,	MAX_STEPS_PUMP0	,	0	,	0	,	RETRY_CURR|FAILED	},
{/*72*/	PULL|FREE_POS	,	1	,	AIR_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*73*/	PUSH|FREE_POS	,	1	,	AIR_STEPS_PUMP0	,	0	,	0	,	FIN_STEP	},
{/*74*/	ACT_NONE	,	1	,	10	,	0	,	0	,	LOG_FLASH	},
{/*75*/	ACT_NONE	,	1	,	10	,	0	,	0	,	LOG_FLASH	},
{/*76*/	ACT_NONE	,	1	,	10	,	0	,	0	,	LOG_FLASH	},
{/*77*/	ACT_NONE	,	1	,	10	,	0	,	0	,	LOG_FLASH	},
{/*78*/	PUSH|FREE_POS	,	1	,	AIR_STEPS_PUMP1	,	0	,	0	,	LOG_FLASH	},
{/*79*/	PULL|FREE_POS	,	1	,	AIR_STEPS_PUMP1	,	0	,	0	,	LOG_FLASH	},


};

static const uint16_t timeDelay_Default[e_max_timedelay] =
{
    100,10,25,8,10,\
    12,1800,3000,5400,120,\
};

static const SPAction spAction_Default[e_max_sp] =
{
    //90
    {5,		 LOG_FLASH|FAILED},
    {6,		 LOG_FLASH|FAILED},
    {7,		 LOG_FLASH|FAILED},
    {8,		 LOG_FLASH|FAILED},
    {1000,		 LOG_FLASH|FAILED},
    {6000,		 LOG_FLASH|FAILED},
    {5000,		 LOG_FLASH|FAILED},
    {6100,		 LOG_FLASH|FAILED},
    {6100,		 LOG_FLASH|FAILED},
    {6200,		 LOG_FLASH|FAILED},
    //100
    {610,		 LOG_FLASH|FAILED},
    {610,		 LOG_FLASH|FAILED},
    {5100,		 LOG_FLASH|FAILED},
    {250,		 LOG_FLASH|FAILED},
    {5200,		 LOG_FLASH|FAILED},
    {6200,		 LOG_FLASH|FAILED},
    {100,		 LOG_FLASH|FAILED},
    {100,		 LOG_FLASH|FAILED},
    {1620,		 LOG_FLASH|FAILED},
    {1500,		 LOG_FLASH|FAILED},
    //110
    {6200,		 LOG_FLASH|FAILED},
    {6200,		 LOG_FLASH|FAILED},
    {6200,		 LOG_FLASH|FAILED},
    {110,		 LOG_FLASH|FAILED},
    {110,		 LOG_FLASH|FAILED},
    {3000,		 LOG_FLASH|FAILED},
    {5,		 LOG_FLASH|FAILED},
    {5,		 LOG_FLASH|FAILED},
    {5,		 LOG_FLASH|FAILED},
    {5,		 LOG_FLASH|FAILED},

};

//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
	NULL_T_DATA_OBJ,
    //1
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[0],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[1],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[2],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[3],	NON_VOLATILE),
	//5
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[4],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[5],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[6],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[7],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[8],	NON_VOLATILE),
	//10
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[9],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[10],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[11],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[12],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[13],	NON_VOLATILE),
	//15
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[14],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[15],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[16],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[17],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[18],	NON_VOLATILE),
	//20
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[19],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[20],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[21],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[22],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[23],	NON_VOLATILE),
	//25
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[24],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[25],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[26],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[27],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[28],	NON_VOLATILE),
	//30
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[29],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[30],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[31],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[32],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[33],	NON_VOLATILE),
	//35
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[34],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[35],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[36],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[37],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&cmdValveDelay[38],	NON_VOLATILE),
	//40
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[0], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[1], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[2], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[3], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[4], NON_VOLATILE),
    //45
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[5], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[6], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[7], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[8], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[9], NON_VOLATILE),
    
	//50
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[10], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[11], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[12], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[13], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[14], NON_VOLATILE),
    //55
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[15], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[16], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[17], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[18], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[19], NON_VOLATILE),
    //60
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[20], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[21], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[22], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[23], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[24], NON_VOLATILE),
    //65
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[25], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[26], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[27], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[28], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[29], NON_VOLATILE),
    //70
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[30], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[31], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[32], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[33], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[34], NON_VOLATILE),
    //75
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[35], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[36], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[37], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[38], NON_VOLATILE),
    CONSTRUCT_STRUCT_PUMPCFG(&pumpCfg[39], NON_VOLATILE),
    //80
    
	CONSTRUCT_SIMPLE_U16(&timeDelay[0],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[1],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[2],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[3],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[4],	NON_VOLATILE),
	//85
	CONSTRUCT_SIMPLE_U16(&timeDelay[5],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[6],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[7],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[8],	NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&timeDelay[9],	NON_VOLATILE),
	//90
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[0],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[1],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[2],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[3],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[4],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	//95
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[5],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[6],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[7],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[8],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[9],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	//100
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[10],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[11],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[12],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[13],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[14],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	//105
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[15],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[16],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[17],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[18],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[19],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	//110
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[20],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[21],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[22],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[23],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[24],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	//115
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[25],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[26],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[27],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[28],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&spAction[29],    sizeof(spAction[0])/sizeof(uint16_t),	NON_VOLATILE),
	//120
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[0][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[1][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[2][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[3][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[4][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    //125
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[5][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[6][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[7][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[8][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[9][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	//130
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[10][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[11][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[12][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[13][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[14][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
	//135
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[15][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[16][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[17][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[18][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&preSubSteps[19][0], sizeof(preSubSteps[0]),	NON_VOLATILE),
     
    
    //140
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[0][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[1][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[2][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[3][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[4][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //145
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[5][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[6][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[7][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[8][0], sizeof(subSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[9][0], sizeof(subSteps[0]),	NON_VOLATILE),
	//150
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[10][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[11][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[12][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[13][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[14][0], sizeof(subSteps[0]),	NON_VOLATILE),
	//155
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[15][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[16][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[17][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[18][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[19][0], sizeof(subSteps[0]),	NON_VOLATILE),
    
    //160
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[20][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[21][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[22][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[23][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[24][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //165
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[25][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[26][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[27][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[28][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[29][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //170
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[30][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[31][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[32][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[33][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[34][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //175
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[35][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[36][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[37][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[38][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[39][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //180
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[40][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[41][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[42][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[43][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[44][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //185
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[45][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[46][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[47][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[48][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[49][0], sizeof(subSteps[0]),	NON_VOLATILE),
    
    //190
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[50][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[51][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[52][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[53][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[54][0], sizeof(subSteps[0]),	NON_VOLATILE),
    //195
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[55][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[56][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[57][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[58][0], sizeof(subSteps[0]),	NON_VOLATILE),
    CONSTRUCT_ARRAY_SIMPLE_U8(&subSteps[59][0], sizeof(subSteps[0]),	NON_VOLATILE),
    
    
    //200
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	//205
    
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
    //210
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[0][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[1][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[2][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[3][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[4][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	//215
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[5][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[6][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[7][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[8][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[9][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	//220
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[10][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[11][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[12][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[13][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[14][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	//225
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[15][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[16][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[17][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[18][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&preMainSteps[19][0], sizeof(preMainSteps[0]),	NON_VOLATILE),
	//230
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[0][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[1][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[2][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[3][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[4][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	//235
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[5][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[6][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[7][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[8][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[9][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	//240
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[10][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[11][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[12][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[13][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[14][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	//245
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[15][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[16][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[17][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[18][0], sizeof(mainSteps[0]),	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(&mainSteps[19][0], sizeof(mainSteps[0]),	NON_VOLATILE),
    
	//250
    
    
    
};



const char* StepName[e_main_step_end] =
{
/*0*/	TO_STR(NONE),
/*1*/	TO_STR(e_open_cera_v),
/*2*/	TO_STR(e_open_air_v),
/*3*/	TO_STR(e_open_samp_v),
/*4*/	TO_STR(e_open_waste_v),
/*5*/	TO_STR(e_open_dichro_v),
/*6*/	TO_STR(e_open_blank_v),
/*7*/	TO_STR(e_open_H2SO4_v),
/*8*/	TO_STR(e_open_HgSO4_v),
/*9*/	TO_STR(e_open_std_v),
/*10*/	TO_STR(e_open_safety_v),
/*11*/	TO_STR(e_open_lock),
/*12*/	TO_STR(e_close_cera_v),
/*13*/	TO_STR(e_close_air_v),
/*14*/	TO_STR(e_close_samp_v),
/*15*/	TO_STR(e_close_waste_v),
/*16*/	TO_STR(e_close_dichro_v),
/*17*/	TO_STR(e_close_blank_v),
/*18*/	TO_STR(e_close_H2SO4_v),
/*19*/	TO_STR(e_close_HgSO4_v),
/*20*/	TO_STR(e_close_std_v),
/*21*/	TO_STR(e_close_safety_v),
/*22*/	TO_STR(e_close_lock),
/*23*/	TO_STR(e_open_relay1),
/*24*/	TO_STR(e_open_relay2),
/*25*/	TO_STR(e_close_relay1),
/*26*/	TO_STR(e_close_relay2),
/*27*/	TO_STR(e_open_dig_fan_v),
/*28*/	TO_STR(e_close_dig_fan_v),
/*29*/	TO_STR(e_open_PCBA_fan_v),
/*30*/	TO_STR(e_close_PCBA_fan_v),
/*31*/	TO_STR(e_open_enclosure_fan_v),
/*32*/	TO_STR(e_close_enclosure_fan_v),
/*33*/	TO_STR(rev),
/*34*/	TO_STR(rev),
/*35*/	TO_STR(rev),
/*36*/	TO_STR(rev),
/*37*/	TO_STR(rev),
/*38*/	TO_STR(rev-for space),
/*39*/	TO_STR(deviceReset),
/*40*/	TO_STR(PUMP_SO4_IN_T+J45UBE),
/*41*/	TO_STR(PUMP_STD0_IN_TUBE),
/*42*/	TO_STR(PUMP_STD1_IN_TUBE),
/*43*/	TO_STR(PUMP_CR6_IN_TUBE),
/*44*/	TO_STR(PUMP_HGSO4_IN_TUBE),
/*45*/	TO_STR(PUMP_SAMPLE_IN_TUBE),
/*46*/	TO_STR(PUMP_AIR_BACK_IN_PIPE),
/*47*/	TO_STR(PUMP_CLEAN_TUBE_TO_UP),
/*48*/	TO_STR(PUMP_WASTE0_IN_TUBE),
/*49*/	TO_STR(PUMP_WASTE1_IN_TUBE),
/*50*/	TO_STR(PUMP_LIQUID_IN_TUBE),
/*51*/	TO_STR(NONE),
/*52*/	TO_STR(PUMP_AIR_DIG_PIPE_IN_TUBE),
/*53*/	TO_STR(PUMP_AIR_FULL),
/*54*/	TO_STR(PUMP_SO4_TO_DIG),
/*55*/	TO_STR(PUMP_STD0_TO_DIG),
/*56*/	TO_STR(PUMP_STD1_TO_DIG),
/*57*/	TO_STR(PUMP_CR6_TO_DIG),
/*58*/	TO_STR(PUMP_HGSO4_TO_DIG),
/*59*/	TO_STR(PUMP_SAMPLE_TO_DIG),
/*60*/	TO_STR(PUMP_CLEAN_TUBE_TO_DOWN),
/*61*/	TO_STR(PUMP_AIR_MIX_AND_EMPTY),
/*62*/	TO_STR(PUMP_CLEAN_TO_SAMPLE_PIPE),
/*63*/	TO_STR(TEST_FOR_MORE_VOLUME),
/*64*/	TO_STR(PUMP_AIR_BACK),
/*65*/	TO_STR(NONE),
/*66*/	TO_STR(NONE),
/*67*/	TO_STR(NONE),
/*68*/	TO_STR(NONE),
/*69*/	TO_STR(NONE),
/*70*/	TO_STR(NONE),
/*71*/	TO_STR(PUMP_RESTORE),
/*72*/	TO_STR(PUMP_CR6_TO_DIG_TEST_PULL),
/*73*/	TO_STR(PUMP_CR6_TO_DIG_TEST_PUSH),
/*74*/	TO_STR(NONE),
/*75*/	TO_STR(NONE),
/*76*/	TO_STR(NONE),
/*77*/	TO_STR(NONE),
/*78*/	TO_STR(PUMP_AIR_TO_PRESSS_BUBBLE),
/*79*/	TO_STR(PUMP_AIR_FROM__BUBBLE),
/*80*/	TO_STR(TIME1),
/*81*/	TO_STR(TIME2),
/*82*/	TO_STR(TIME3),
/*83*/	TO_STR(TIME4),
/*84*/	TO_STR(TIME5),
/*85*/	TO_STR(TIME6),
/*86*/	TO_STR(TIME7),
/*87*/	TO_STR(TIME8),
/*88*/	TO_STR(TIME9),
/*89*/	TO_STR(TIME10),
/*90*/	TO_STR(LED0_DETECT_ENABLE),
/*91*/	TO_STR(LED0_DETECT_DISABLE),
/*92*/	TO_STR(LED1_DETECT_ENABLE),
/*93*/	TO_STR(LED1_DETECT_DISABLE),
/*94*/	TO_STR(HEAT_TO_PREHEAT),
/*95*/	TO_STR(HEAT_TO_MEASURE),
/*96*/	TO_STR(HEAT_TO_DIG),
/*97*/	TO_STR(HEAT_TO_STEP2),
/*98*/	TO_STR(COOLING_TO_100C),
/*99*/	TO_STR(DEVICE_DIG_INITIALIZE_COOLING),
/*100*/	TO_STR(Measure(multi sample + avg)),
/*101*/	TO_STR(blank),
/*102*/	TO_STR(PRE_COOLING_BEFORE_SAMPLING),
/*103*/	TO_STR(PUMP_RESTORE_CHECK),
/*104*/	TO_STR(HEAT_TO_CLEAN),
/*105*/	TO_STR(COOLING_BEFORE_IDLE),
/*106*/	TO_STR(VALVE_RESETTING MPP),
/*107*/	TO_STR(VALVE_RESETTING HPP),
/*108*/	TO_STR(REMOVE_BURBLE_LED0_TIMEDELAY),
/*109*/	TO_STR(REMOVE_BURBLE_LED0),
/*110*/	TO_STR(REMOVE_BURBLE_LED1),
/*111*/	TO_STR(HEAT_INCREASE_DETECT),
/*112*/	TO_STR(SENSOR_GAIN_UPDATE),
/*113*/	TO_STR(UPDATE_CALI_BLANK),
/*114*/	TO_STR(UPDATE_CALI_SLOPE),
/*115*/	TO_STR(IDLE),
/*116*/	TO_STR(CHK_LEAKAGE_DISABLE),
/*117*/	TO_STR(CHK_SAFETY_LOCK_ENABLE),
/*118*/	TO_STR(CHK_SAFETY_LOCK_DISABLE),
/*119*/	TO_STR(CHK_LEAKAGE_ENABLE),
/*120*/	TO_STR(CLEAN_DIG_STD0),
/*121*/	TO_STR(CLEAN_DIG_STD1),
/*122*/	TO_STR(EMPTY_TUBE_AIR),
/*123*/	TO_STR(WASTE_TO_TUBE_LED0_DRAIN),
/*124*/	TO_STR(WASTE_TO_TUBE_LED1_DRAIN),
/*125*/	TO_STR(CLEAN_DIG_SAMPLE),
/*126*/	TO_STR(TO_PUMP_CR6_TEST_ABOVE),
/*127*/	TO_STR(TO_PUMP_CR6_TEST_BELOW),
/*128*/	TO_STR(VALVE_RESTORE),
/*129*/	TO_STR(CLEAN_TUBE_AIR),
/*130*/	TO_STR(PUMP_INITIALIZE),
/*131*/	TO_STR(CLEAN_PUMP_UP_AND_DAWN5),
/*132*/	TO_STR(CLEAN_PUMP_UP_AND_DAWN7),
/*133*/	TO_STR(TO_PUMP_SO4),
/*134*/	TO_STR(TO_PUMP_HG),
/*135*/	TO_STR(TO_PUMP_STD0),
/*136*/	TO_STR(TO_PUMP_STD1),
/*137*/	TO_STR(TO_PUMP_SAMPLE),
/*138*/	TO_STR(TO_PUMP_CR6),
/*139*/	TO_STR(TO_PUMP_CR6_TEST),
/*140*/	TO_STR(TO_DIG_SO4_S),
/*141*/	TO_STR(TO_DIG_HG_S),
/*142*/	TO_STR(TO_DIG_STD0_S),
/*143*/	TO_STR(TO_DIG_STD1_S),
/*144*/	TO_STR(TO_DIG_SAMPLE_S),
/*145*/	TO_STR(TO_DIG_CR6_S),
/*146*/	TO_STR(MIX_WITH_AIR_FAST),
/*147*/	TO_STR(MIX_WITH_AIR_SLOW),
/*148*/	TO_STR(CLEAN_DIG_STD0_M),
/*149*/	TO_STR(CLEAN_DIG_STD1_M),
/*150*/	TO_STR(EMPTY_TUBE_AIR_S),
/*151*/	TO_STR(NONE),
/*152*/	TO_STR(TO_TUBE_STD0_FLUSH),
/*153*/	TO_STR(TO_TUBE_STD1_FLUSH),
/*154*/	TO_STR(TO_TUBE_SAMPLE_FLUSH),
/*155*/	TO_STR(TO_TUBE_CR6_FLUSH),
/*156*/	TO_STR(AIR_TO_PIPE_SO4),
/*157*/	TO_STR(AIR_TO_PIPE_STD0),
/*158*/	TO_STR(AIR_TO_PIPE_STD1),
/*159*/	TO_STR(AIR_TO_PIPE_HG),
/*160*/	TO_STR(AIR_TO_PIPE_SAMPLE),
/*161*/	TO_STR(AIR_TO_PIPE_CR),
/*162*/	TO_STR(CLEAN_TUBE_STD0),
/*163*/	TO_STR(CLEAN_TUBE_SO4),
/*164*/	TO_STR(CLEAN_TUBE_110C_SO4),
/*165*/	TO_STR(NONE),
/*166*/	TO_STR(WASTE_TO_TUBE_LED0_DRAIN_S),
/*167*/	TO_STR(WASTE_TO_TUBE_LED1_DRAIN_S),
/*168*/	TO_STR(NONE),
/*169*/	TO_STR(WASTE_TO_DRAIN_AFTER_MEASURE),
/*170*/	TO_STR(WASTE_TO_DRAIN_PRE_MEASURE),
/*171*/	TO_STR(NONE),
/*172*/	TO_STR(EMPTY_STD0_PIPE_AIR),
/*173*/	TO_STR(EMPTY_STD1_PIPE_AIR),
/*174*/	TO_STR(CLEAN_TUBE_STD1),
/*175*/	TO_STR(NONE),
/*176*/	TO_STR(CLEAN_TUBE_SAMPLE),
/*177*/	TO_STR(CLEAN_DIG_SAMPLE_S),
/*178*/	TO_STR(EMPTY_SAMPLE_PIPE_AIR),
/*179*/	TO_STR(NONE),
/*180*/	TO_STR(INITIALIZE_INSTRUMENT),
/*181*/	TO_STR(PUMP_TO_WASTE),
/*182*/	TO_STR(NONE),
/*183*/	TO_STR(NONE),
/*184*/	TO_STR(TO_DIG_SAMPLE_M),
/*185*/	TO_STR(TO_DIG_SO4_M),
/*186*/	TO_STR(TO_DIG_HG_M),
/*187*/	TO_STR(TO_DIG_CR6_M),
/*188*/	TO_STR(TO_DIG_STD0_M),
/*189*/	TO_STR(TO_DIG_STD1_M),
/*190*/	TO_STR(EMPTY_DIG_LED0_M),
/*191*/	TO_STR(CLEAN_DIG_SAMPLE_M),
/*192*/	TO_STR(EMPTY_DIG_LED1_M),
/*193*/	TO_STR(EMPTY_TUBE_AIR_M),
/*194*/	TO_STR(EMPTY_SAMPLE_PIPE),
/*195*/	TO_STR(EMPTY_HG_PIPE),
/*196*/	TO_STR(EMPTY_CR6_PIPE),
/*197*/	TO_STR(EMPTY_SO4_PIPE),
/*198*/	TO_STR(EMPTY_STD1_PIPE),
/*199*/	TO_STR(EMPTY_STD0_PIPE),
/*200*/	TO_STR(blank_drain),
/*201*/	TO_STR(std1_drain),
/*202*/	TO_STR(sample_drain),
/*203*/	TO_STR(NONE),
/*204*/	TO_STR(NONE),
/*205*/	TO_STR(NONE),
/*206*/	TO_STR(NONE),
/*207*/	TO_STR(NONE),
/*208*/	TO_STR(NONE),
/*209*/	TO_STR(NONE),
/*210*/	TO_STR(measure_std0),
/*211*/	TO_STR(measure_std1),
/*212*/	TO_STR(CR6_VOLUME_TEST),
/*213*/	TO_STR(NONE),
/*214*/	TO_STR(NONE),
/*215*/	TO_STR(measure_sample),
/*216*/	TO_STR(NONE),
/*217*/	TO_STR(NONE),
/*218*/	TO_STR(NONE),
/*219*/	TO_STR(std1_pre),
/*220*/	TO_STR(std0_pre),
/*221*/	TO_STR(std1_samplein),
/*222*/	TO_STR(std0_samplein),
/*223*/	TO_STR(FLUSH),
/*224*/	TO_STR(Sample_In),
/*225*/	TO_STR(Cr6_In),
/*226*/	TO_STR(HG_In),
/*227*/	TO_STR(SO4_IN),
/*228*/	TO_STR(Meas_Pre),
/*229*/	TO_STR(meas_post),
/*230*/	TO_STR(calibration),
/*231*/	TO_STR(flush),
/*232*/	TO_STR(NONE),
/*233*/	TO_STR(measure),
/*234*/	TO_STR(NONE),
/*235*/	TO_STR(measure_std0),
/*236*/	TO_STR(measure_std1),
/*237*/	TO_STR(NONE),
/*238*/	TO_STR(NONE),
/*239*/	TO_STR(NONE),

};

typedef struct
{
	uint8_t dir;
	uint8_t speed;
	uint16_t pos;
	uint16_t stepsEachTime;
    uint16_t preDefineTime;
    uint16_t preDefineSteps;
    uint16_t errorAction;
}PumpConfig;

static const  T_DATACLASS _ClassList[]=
{
    //lint -e545  -e651
 	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1_0,fileID_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2_0,fileID_Default),
    CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1_1,fileID_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2_1,fileID_Default),
    
	CONSTRUCTOR_DC_STATIC_CONSTDEF(subSteps,subSteps_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(mainSteps,mainSteps_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(preSubSteps,preSubSteps_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(preMainSteps,preMainSteps_Default),
    
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pumpCfg,pumpCfg_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(timeDelay,timeDelay_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(spAction,spAction_Default),
};


//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT flowCfg =
{
    (uint8_t*)"flowCfg",
    &_State,
    &_Semaphore,
    
    _ObjList,
    //lint -e{778}
    sizeof(_ObjList)/sizeof(_ObjList[0])-1,
    _ClassList,
    sizeof(_ClassList)/sizeof(_ClassList[0]),
    
    Initialize_FlowCfg, // will be overloaded
    LoadRomDefaults_T_UNIT,
    Get_T_UNIT,
	Put_FlowCfg,
    Check_T_UNIT,
    GetAttributeDescription_T_UNIT,
    GetObjectDescription_T_UNIT,
    GetObjectName_T_UNIT
};



uint16_t Initialize_FlowCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
    uint16_t result = OK;
    //lint -e{746}
    assert(me==&flowCfg);
    result = Initialize_T_UNIT(me,typeOfStartUp);
    if(result==OK)
    {
        if((typeOfStartUp & INIT_DATA) != 0)
        {
            //todo?
            if( (fileId1_0 != fileID_Default) || (fileID_Default != fileId2_0) || (fileId1_1 != fileID_Default) || (fileID_Default != fileId2_1) )
            {
                (void)me->LoadRomDefaults(me,ALL_DATACLASSES);
                TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);
            }
        }
        
    }
    return result;
}

uint16_t Put_FlowCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &flowCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	UpdateTimeCostVal();
	return result;
}
const char* GetStepName(uint16_t chn)
{
	static const char defaultStepName[] = "UNKNOWN_NAME";
	static const char nullStepName[] = "\0\0";
	const char* adr = NULL;
    
	if(chn == 0)
		adr = nullStepName;
	else if(chn <= e_main_step_end)
		adr = StepName[chn];
    
	if(adr == NULL)
		adr = &defaultStepName[0];
	return adr;
}



uint32_t GetDuringMs_MicroStep(uint16_t subStep)
{
    if(IN_RANGE(subStep, e_timedelay_start, e_timedelay_end))
	{
		return timeDelay[subStep-e_timedelay_start]*TIME_UNIT;
	}
	else if(IN_RANGE(subStep, e_valve_start, e_valve_end))
	{
		return VALVE_DURING_TIME;
	}
	else if(IN_RANGE(subStep, e_pump_start, e_pump_end))
	{
		return PUMP_TIME_MAX;
	}
	else if(IN_RANGE(subStep, e_sp_ctrl_start, e_sp_ctrl_end))
	{
        return spAction[subStep-e_sp_ctrl_start].timeExpect*TIME_UNIT; 
	}
    else
    {
        return 0;
    }
}



