/*
 * t_dataclass.c
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_dataclass.h"


//--------------------------------------------------------------------------------------------------
/*!
 \brief  That method loads the dynamic-initializers placed in ROM into RAM (Dynamic-Memory)
 <pre>
	not allowed in mode "NOT_INITIALIZED", the state will switched to "INACTIVE" or "ERROR_STATE"

	+---------------------------------------------+    +-----------------------------+
	|dynamic initializer                          |--->|dynamic data                 |
	+---------------------------------------------+    +-----------------------------+

  </pre>
 \param  me pointer of T_DATACLASS
 \return error-code
 <pre>
   OK                      operation was successful
   DECLINE                 DEBUG, operation isn't permitted in the aktual unit-state
 </pre>
 */
//--------------------------------------------------------------------------------------------------
uint16_t LoadRomDefaults_Dynamic_T_DATACLASS(const T_DATACLASS *me)
{
	// check for zero-pointers
	VIP_ASSERT(me);
	//lint -e{619} loss of precision (arg. no. 2) (pointer to pointer) is OK
	(void)memcpy(me->ptrData.dcDynamic, me->ptrInitializer, me->length);
	return OK;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  That method loads the initializers placed in ROM into static-memory
  <pre>
	not allowed in mode "NOT_INITIALIZED", the state will switched to "INACTIVE" or "ERROR_STATE"

	+----------------------------------------+    +------------------------+
	|static initializer                      |--->+static data             |
	+----------------------------------------+    +------------------------+

  </pre>
 \param  me pointer of T_DATACLASS
 \return error-code
 <pre>
   OK                      operation was successful
   DECLINE                 DEBUG, operation isn't permitted in the actual unit-state
 </pre>
 */
//--------------------------------------------------------------------------------------------------
uint16_t LoadRomDefaults_Static_T_DATACLASS(const T_DATACLASS *me)
{
	// check for zero-pointers
	VIP_ASSERT(me);
	return PutData_NV_MEM(me->ptrData.dcStatic, me->ptrInitializer, me->length);
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  NoFunction -- Returns always OK
 <pre>
 </pre>
 \param  me pointer of T_DATACLASS
 \return error-code
 <pre>
   OK                      operation was successful
   DECLINE                 DEBUG, operation isn't permitted in the aktual unit-state
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t NoFunction_T_DATACLASS(const T_DATACLASS *me)
{
	VIP_ASSERT(me);
	return OK;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  NoFunction -- Returns always OK
 <pre>
 </pre>
 \param  dst copy dst address in ram
 \param  src copy source address
 \param  len copy length
 \return error-code
 <pre>
   OK                      operation was successful
   DECLINE                 DEBUG, operation isn't permitted
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t PutData_NV_MEM(void* dst,const void* src,uint16_t len)
{
	memcpy(dst,src,len);
	return Trigger_EEPSave(dst, len, SYNC_CYCLE);

}
