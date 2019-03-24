/*
 * t_data_obj.h
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#ifndef SRC_T_DATA_OBJ_H_
#define SRC_T_DATA_OBJ_H_

#include <stdio.h>
#include <stdbool.h>
#include "simple_type.h"

#pragma GCC diagnostic ignored "-Wpadded"
//----------------------------------------------------------------------------------------------------------
/*! return-codes of a data-object, compatible to T_UNIT_RETURNCODES
    for each warning-code there must be one error-code. The distance between the warning-code
    and WARNING must be the same as the distance between error code and ERROR.
*/
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
	DEVICE_BUSY_STATE,
    // this errorcodes occures if proper function of the subsystem ist guaranteed any longer
    // e.g. the subsystem attributes are corrupted.
    //@@ maybe an exception for fatal errors?
    FATAL_ERROR=96,         						//!< start of fatal-errors
    ERROR_NV_STORAGE=96,    						//!< attributes inside the nv-storage are corrupted
    ERROR_RAM_STORAGE,      						//!< attributes inseid the ram are corrupted
    ERROR_HW,                							//!< hardware used by the subsystem doesn't work
	ERROR_DATA_LENGTH,

} FRESULT_UNIT;


//----------------------------------------------------------------------------------------------------------
//! enumerations
enum E_STORAGE_QUALIFIER
{
    RAM,       //!< the data-object is resided in RAM
    NON_VOLATILE,  //!<                    resided in non-volatile-storage
    ROM,           //!< the data-object is resided in ROM --> READ_ONLY, no businessrules
    READONLY_RAM,  //!< resided in RAM but readonly
    READONLY_NOV   //!< resided in non-volatile-storage but readonly
};

enum E_TYPE_QUALIFIER
{
    SIMPLE,        //!< the DATA-OBJ is a simple data type e.g. int16_t, FLOAT...
    STRUCT,        //!< structure of simple data type
    ARRAY          //!< array of simple data type
};


enum E_ATTRIB_IDX
{
    WHOLE_OBJECT=-1,
    ATTRIB_0,
    ATTRIB_1,
    ATTRIB_2,
    ATTRIB_3,
    ATTRIB_4,
    ATTRIB_5,
    ATTRIB_6,
    ATTRIB_7,
    ATTRIB_8,
    ATTRIB_9
};

// The forward declaration of struct _T_DATA_OBJ is necessary for struct _T_DATA_OBJ_CLASS
struct _T_DATA_OBJ;

//----------------------------------------------------------------------------------------------------------
//! this type is used by GetObjDescripton
typedef struct _T_DO_OBJECT_DESCRIPTION
{
    uint16_t                 objectLength;
    uint16_t                 numberOfAttributes;
    const uint8_t *       aryAttribDataTypes;
    enum E_TYPE_QUALIFIER    typeQualifier;
} T_DO_OBJECT_DESCRIPTION;


typedef struct _T_DO_ATTRIB_RANGE
{
    bool   rulesAreActive;   //!< FLASE: this attribute will not be validated --> no range-info
    uint8_t dataType;         //!< type info for the following union
    T_RANGE_CONTAINER range;  //!< union that hold the range-info in dependency to dataType
    uint32_t adr;
    uint16_t len;
} T_DO_ATTRIB_RANGE;

//----------------------------------------------------------------------------------------------------------
//! this structure describes the part of the class T_DATA_OBJ that is related only to the class
typedef struct _T_DATA_OBJ_CLASS
{
    //**************************************************************************************************
    // class attributes
    //**************************************************************************************************
    const uint8_t * aryAttribDataTypes;

    //**************************************************************************************************
    // public methods
    //**************************************************************************************************


    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  check object or attribute value against the object businessrules. the nearest possible
             value will be returned.
             The value must have the format of the external view
     \param  the nearest possible object or attribute value will be returned in ptrValue
     \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
     \return error-code
     <pre>
         OK                    operation was successful
         LESSTHAN_RANGEMIN     at least one value is less than its minimum value
         GREATERTHAN_RANGEMAX  at least one value is greater than its maximum
         SPAN_TO_SMALL         then span between two values is to small
         NOT_ON_GRID           at least one value is not a multiple of its increment
         ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
         FATAL_ERROR           object outside nv-mem range
     </pre>
     \warning starting with Revision 2.1.0 Check is not longer privat! Thus the zero-pointer as a mark
              for not using Check is not loner allowed!
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t   (*Check)(const struct _T_DATA_OBJ *me,
                        void * ptrValue,
                        int16_t attributeIndex );


    //**************************************************************************************************
    // public methods
    //**************************************************************************************************

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  get the actual object or attribute value.
             Access to memory protected with a resource semaphore will be supported if the pointer to this
             semaphore is not zero.

     <pre>
             1. check attributeIndex
             2. copy data
             3. transform to external view (function call)
             4. return
     </pre>

     \param  the new object or attribute value will be returned in ptrValue
     \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
     \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
     \return error-code
     <pre>
         OK                    operation was successful
         LESSTHAN_RANGEMIN     at least one value is less than its minimum value
         GREATERTHAN_RANGEMAX  at least one value is greater than its maximum
         SPAN_TO_SMALL         then span between two values is to small
         NOT_ON_GRID           at least one value is not a multiple of its increment
         ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
         FATAL_ERROR           object outside nv-mem range
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t   (*Get)(const struct _T_DATA_OBJ *me,
                      void * ptrValue,
                      int16_t attributeIndex,
                      OS_RSEMA * ptrDataSemaphore);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  put a new object or attribute value into ram, if no businessrules are voilated
             Access to memory protected with a resource semaphore will be supported if the pointer to this
             semaphore is not zero.
     <pre>
             1. check storageQualifier
             2. check attributeIndex
             3. check businessrules        (function call)
             4. transform to internal view (function call)
             5. copy data if no error until now
             6. transform to external view (function call)
             7. return
     </pre>

     \param  ptrValue the new object or attribute value; if return!=OK than return the
             nearest possible value in ptrValue
     \param  attributeIndex <0 --> Object; >=0 --> Attribute
     \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
     \return error-code
     <pre>
         OK                    operation was successful
         LESSTHAN_RANGEMIN     at least one value is less than its minimum value
         GREATERTHAN_RANGEMAX  at least one value is greater than its maximum
         SPAN_TO_SMALL         then span between two values is to small
         NOT_ON_GRID           at least one value is not a multiple of its increment
         ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
         FATAL_ERROR           object outside nv-mem range
         READ_ONLY_ERR         parameter is read only
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t   (*Put)(const struct _T_DATA_OBJ *me,
                      void * ptrValue,
                      int16_t attributeIndex,
                      OS_RSEMA * ptrDataSemaphore);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  get information about a single attribute
     \param  the attribute information will be returned in ptrDescriptor
     \param  >=0 --> Attribute-info else error
     \return error-code
     <pre>
         OK                    operation was successful
         ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*GetAttributeDescription)(const struct _T_DATA_OBJ *me,
                               T_DO_ATTRIB_RANGE * ptrDescriptor,
                               uint16_t attributeIndex);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  get information about the object
     \param  the object description will be returned in ptrDescriptor
     \param  ==0 --> object-info else error
     \return error-code
     <pre>
         OK                    operation was successful
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t  (*GetObjectDescription)(const struct _T_DATA_OBJ *me,
                                   T_DO_OBJECT_DESCRIPTION * ptrDescriptor);

} T_DATA_CLASS;



//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ
extern const T_DATA_CLASS cgClassInitializer_TDATAOBJ;



//----------------------------------------------------------------------------------------------------------
//! definition of class data-obj; class description + object related description
typedef struct _T_DATA_OBJ
{
	const char* nameDesp;
    // privat attributes
    void * ptrValue;        /*!< data-object value inside RAM;  for less code
                                         consumption and less cpu-load. Disadvantage: READONLY
                                         objects reside in RAM */

    uint16_t numberOfAttributes;    /*!<  */
    uint16_t objectLength;          //!<

    enum E_STORAGE_QUALIFIER storageQualifier;//!< qualifies the kind of storage where the object resides
    enum E_TYPE_QUALIFIER    typeQualifier;   //!< qualifies the kind of data-type

    const void  *  ptrDescription;  //!< data needed by the check-method for checking

    // class specific methods and attributes
    T_DATA_CLASS  const* pClass;

} T_DATA_OBJ;

//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ.
 \param  w ptrValue; the address will be casted to (void *)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_DATAOBJ(w,y) { (void *)(w),1,1,y,SIMPLE, 0, &cgClassInitializer_TDATAOBJ }

//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type array T_DATA_OBJ.
 \param  w ptrValue; the address will be casted to (void *)!
 \param  x array-length
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_ARRAY_DATAOBJ(w,x,y) { (void *)(w),x,x,y,ARRAY, 0, &cgClassInitializer_TDATAOBJ }

//----------------------------------------------------------------------------------------------------------
//! prototypes of the method implementations of T_DATA_OBJ



//--------------------------------------------------------------------------------------------------
/*!
 \brief  dummy function; returns OK
 \param  me; T_DATA_OBJ itself
 \param  ptrValue; the nearest possible object or attribute value will be returned in ptrValue
 \param  attributeIndex; ==-1 --> Object; >=0 --> Attribute
 \return error-code allways OK
 \warning starting with Revision 2.1.0 Check is not longer privat! Thus the zero-pointer as a mark
          for not using Check is not loner allowed!
*/
//--------------------------------------------------------------------------------------------------
uint16_t   Check_DATAOBJ(const struct _T_DATA_OBJ *me,
                         void * ptrValue,
                         int16_t attributeIndex );

//--------------------------------------------------------------------------------------------------
/*!
 \brief  get the actual object or attribute value.
         Access to memory protected with a resource semaphore will be supported if the pointer to this
         semaphore is not zero.
         Objectes resided in ROM are not supported.

 <pre>
         1. check attributeIndex
         2. copy data
         3. transform to external view (function call)
         4. return
 </pre>

 \param  me; T_DATA_OBJ itself
 \param  ptrValue; the new object or attribute value will be returned in ptrValue
 \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
 \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
 \return error-code
 <pre>
     OK                    operation was successful
     LESSTHAN_RANGEMIN     at least one value is less than its minimum value
     GREATERTHAN_RANGEMAX  at least one value is greater than its maximum
     SPAN_TO_SMALL         then span between two values is to small
     NOT_ON_GRID           at least one value is not a multiple of its increment
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
     FATAL_ERROR           object outside nv-mem range
 </pre>
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Get_DATAOBJ(const T_DATA_OBJ *me,
                     void * ptrValue,
                     int16_t attributeIndex,
                     OS_RSEMA * ptrDataSemaphore);

//--------------------------------------------------------------------------------------------------
/*!
 \brief  put a new object or attribute value into ram, if no businessrules are voilated
         Access to memory protected with a resource semaphore will be supported if the pointer to this
         semaphore is not zero.
 <pre>
         1. check storageQualifier
         2. check attributeIndex
         3. check businessrules        (function call)
         4. transform to internal view (function call)
         5. copy data if no error until now
         6. transform to external view (function call)
         7. return
 </pre>

 \param  me; T_DATA_OBJ itself
 \param  ptrValue; the new object or attribute value; if return!=OK than return the
         nearest possible value in ptrValue
 \param  attributeIndex <0 --> Object; >=0 --> Attribute
 \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Put_DATAOBJ(const T_DATA_OBJ *me,
                     void * ptrValue,
                     int16_t attributeIndex,
                     OS_RSEMA * ptrDataSemaphore);

//--------------------------------------------------------------------------------------------------
/*!
 \brief  Reads the addressed attribute range-information
 \param  me; T_DATA_OBJ itself
 \param  ptrDescriptor range-information
 \param  attributeIndex ==-1 --> putObject; >=0 --> putAttribute
 \return error-code
 <pre>
     OK                    operation was successful
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
 </pre>
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetAttributeDescription_DATAOBJ(const T_DATA_OBJ *me,
                                T_DO_ATTRIB_RANGE * ptrDescriptor,
                                uint16_t attributeIndex);

//--------------------------------------------------------------------------------------------------
/*!
 \brief  Read object description
 \param  me; T_DATA_OBJ itself
 \param  ptrDescriptor object description
 \return error-code
 <pre>
     OK                    operation was successful
 </pre>
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetObjectDescription_DATAOBJ(const struct _T_DATA_OBJ *me,
                                   T_DO_OBJECT_DESCRIPTION * ptrDescriptor);


#pragma GCC diagnostic pop
#endif /* SRC_T_DATA_OBJ_H_ */
