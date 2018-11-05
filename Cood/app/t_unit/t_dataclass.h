/*
 * t_dataclass.h
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATACLASS_H_
#define T_UNIT_T_DATACLASS_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "simple_type.h"
#include "t_unit.h"
//--------------------------------------------------------------------------------------------------
// prototypes
uint16_t LoadRomDefaults_Dynamic_T_DATACLASS(const T_DATACLASS *me);
uint16_t LoadRomDefaults_Static_T_DATACLASS(const T_DATACLASS *me);
uint16_t NoFunction_T_DATACLASS(const T_DATACLASS *me);
uint16_t LoadRomDefaults_StaticStatDef_T_DATACLASS(const T_DATACLASS *me);




//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; use it for initializing data-classes of the type CONSTANT
 \param  d structure variable that holds the constant-data
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCTOR_DC_CONSTANT(d) { sizeof(d), \
                                     (const void *)0, \
                                     &(d), \
                                     (const void *)0, \
                                     NoFunction_T_DATACLASS }


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; use it for initializing data-classes of the type DYNAMIC
 \param  d structure variable that holds the dynamic-data
 \param  i structure variable that holds the initializer for dynamic-data
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCTOR_DC_DYNAMIC(d,i) { sizeof(d), \
                                      (const void *)&(i), \
									  .ptrData.dcDynamic =((void *)(&(d))), \
									  NULL, \
                                      LoadRomDefaults_Dynamic_T_DATACLASS}



//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; use it for initializing data-classes of the type STATIC_FREQUENT_CONST_DEFAULT
 \param  d structure variable that holds the static-freq-constdef-data
 \param  i structure variable that holds the initializer for static-freq-constdef-data
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCTOR_DC_STATIC_CONSTDEF(d,i) { sizeof(d), \
                                              (const void *)&(i), \
											  .ptrData.dcConstant =((const void *)(&(d))), \
											  NULL, \
                                              LoadRomDefaults_Static_T_DATACLASS }



uint16_t PutData_NV_MEM(void* dst,const void* src,uint16_t len);
#endif /* T_UNIT_T_DATACLASS_H_ */
