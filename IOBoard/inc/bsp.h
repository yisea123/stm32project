/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "assert.h"
#include "cmsis_os.h"


typedef enum
{
    OK = 0,                 //!< operation was successful

    // this warnings are allowed to ignore, cause the subsystem isn't influenced
    WARNING=1,             							//!< ignoreable codes
    LESSTHAN_RANGEMIN=1,   						//!< at least one value is less than its minimum value
    GREATERTHAN_RANGEMAX,  		 			//!< at least one value is greater than its maximum
    RANGE_TOO_LOW,          						//!< one value of range parameter is less than the allowed minimum value
    RANGE_TOO_HIGH,      			   				//!< one value of range parameter is greater than the maximum
    UPPER_RANGE_TOO_LOW,    					//!< upper value of range parameter is less than the allowed minimum value
    UPPER_RANGE_TOO_HIGH,  			 			//!< upper value of range parameter is greater than the maximum
    LOWER_RANGE_TOO_LOW,   		 			//!< lower value of range parameter is less than the allowed minimum value
    LOWER_RANGE_TOO_HIGH,  		 			//!< lower value of range parameter is greater than the maximum
    SPAN_TO_SMALL,         			 				//!< then span between two values is too small
    NOT_ON_GRID,            							//!< at least one value is not a multiple of its increment
    RULE_VIOLATION,        		 					//!< this shows a violation of one or more businessrules
    READ_ONLY,             		 					//!< written parameter is read only
    SENSOR_INTERFACE_ARM_VIOLATION,
    WRONG_STATE,            							//!< written parameter is read only in current subsystem state

    // this errors occur only while writing a parameter, the value was not written to the parameter
    ERROR_CODES=32,               			    			//!< start of not ignoreable codes
    WRITE_ERROR=32,             						//!< write access denied
    LESSTHAN_RANGEMIN_ERR=32,  		 		//!< at least one value is less than its minimum value
    GREATERTHAN_RANGEMAX_ERR, 		  		//!< at least one value is greater than its maximum
    RANGE_TOO_LOW_ERR,        		  			//!< one value of range parameter is less than the allowed minimum value
    RANGE_TOO_HIGH_ERR,       		  			//!< one value of range parameter is greater than the maximum
    UPPER_RANGE_TOO_LOW_ERR,  			  		//!< upper value of range parameter is less than the allowed minimum value
    UPPER_RANGE_TOO_HIGH_ERR, 			  		//!< upper value of range parameter is greater than the maximum
    LOWER_RANGE_TOO_LOW_ERR,   				//!< lower value of range parameter is less than the allowed minimum value
    LOWER_RANGE_TOO_HIGH_ERR, 		 	 	//!< lower value of range parameter is greater than the maximum
    SPAN_TO_SMALL_ERR,          					//!< then span between two values is too small
    NOT_ON_GRID_ERR,            						//!< at least one value is not a multiple of its increment
    RULE_VIOLATION_ERR,        			 			//!< this shows a violation of one or more businessrules
    READ_ONLY_ERR,              						//!< written parameter is read only
    WRONG_STATE_ERR,            					//!< written parameter is read only in current subsystem state
    EXCESS_CORRECTION_ATTEMPED_ERR,
    APPLIED_PROCESS_TOO_LOW_ERR,
    APPLIED_PROCESS_TOO_HIGH_ERR,
    SENSOR_INTERFACE_COMMUNICATION_ERR,		//!< fe hardware is not available due to an errer
    NEW_LRV_URV_OUTSIDE_SENSOR,
    BOTH_LRV_URV_OUTSIDE_LIMIT,
	BUSY_TIME_OUT,

    // this errorcodes are impossible if the programm work properly. during debuging
    // this codes shold stop the program-executing immediately
    DEBUG_ERROR=64,  						       //!< beginning of implementation-errors
    DECLINE=64,             							//!< DEBUG, operation isn't permitted in the aktual unit-state
	ILLEGAL_SUB_IDX,        							//!< DEBUG, unknown Object
    ILLEGAL_OBJ_IDX,        							//!< DEBUG, unknown Object
    ILLEGAL_ATTRIB_IDX,     						//!< DEBUG, unknown Attribute
    METHOD_NOT_SUPPORTED,   					//!< DEBUG, this data_object does not support the called method
    STATE_DENIED,           							//!< DEBUG, actual state denied the requested state
    ILLEGAL_STATE,          							//!< DEBUG, the requestet state is illegal
    ILLEGAL_DATACLASS_INDEX,					//!< DEBUG, unknown Data-Class
	ERROR_SPI_DATA,
    // this errorcodes occures if proper function of the subsystem ist guaranteed any longer
    // e.g. the subsystem attributes are corrupted.
    //@@ maybe an exception for fatal errors?
    FATAL_ERROR=96,         						//!< start of fatal-errors
    ERROR_NV_STORAGE=96,    						//!< attributes inside the nv-storage are corrupted
    ERROR_RAM_STORAGE,      						//!< attributes inseid the ram are corrupted
    ERROR_HW                							//!< hardware used by the subsystem doesn't work

} FRESULT;

#pragma GCC diagnostic ignored "-Wpadded"



typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
} PinInst __attribute__ ((aligned (2)));



#endif
