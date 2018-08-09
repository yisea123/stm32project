/*
 * t_data_obj_simple.h
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_SIMPLE_H_
#define T_UNIT_T_DATA_OBJ_SIMPLE_H_


#include <stdio.h>
#include <stdbool.h>

#include "simple_type.h"

//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::SIMPLE
extern const T_DATA_CLASS cgClassInitializer_SIMPLE;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::SIMPLE
 \param  v the address will be casted to (void *)!
 \param  x simple type like ST_U8, ST_I16 ....
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_SIMPLE(v,x,y) { /*lint -e(643) allow loss of precision in pointer cast */ \
                                  #v,(void *)(v),1,SIZE_##x,y,SIMPLE, \
                                  (void  const*)(x), &cgClassInitializer_SIMPLE }

//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::SIMPLE
 \param  x the address will be casted to (void *)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_SIMPLE_U8(x,y)       CONSTRUCT_SIMPLE(x,ST_U8,y)
#define CONSTRUCT_SIMPLE_U16(x,y)      CONSTRUCT_SIMPLE(x,ST_U16,y)
#define CONSTRUCT_SIMPLE_U32(x,y)      CONSTRUCT_SIMPLE(x,ST_U32,y)
#define CONSTRUCT_SIMPLE_I32(x,y)      CONSTRUCT_SIMPLE(x,ST_I32,y)
#define CONSTRUCT_SIMPLE_T32(x,y)      CONSTRUCT_SIMPLE(x,ST_T32,y)
#define CONSTRUCT_SIMPLE_U64(x,y)      CONSTRUCT_SIMPLE(x,ST_U64,y)
#define CONSTRUCT_SIMPLE_I64(x,y)      CONSTRUCT_SIMPLE(x,ST_I64,y)
#define CONSTRUCT_SIMPLE_I16(x,y)      CONSTRUCT_SIMPLE(x,ST_I16,y)
#define CONSTRUCT_SIMPLE_E8(x,y)       CONSTRUCT_SIMPLE(x,ST_E8,y)
#define CONSTRUCT_SIMPLE_BIT8(x,y)     CONSTRUCT_SIMPLE(x,ST_BIT8,y)
#define CONSTRUCT_SIMPLE_BIT16(x,y)     CONSTRUCT_SIMPLE(x,ST_BIT16,y)
#define CONSTRUCT_SIMPLE_FLOAT(x,y)    CONSTRUCT_SIMPLE(x,ST_FLOAT,y)
#define CONSTRUCT_SIMPLE_DOUBLE(x,y)   CONSTRUCT_SIMPLE(x,ST_DOUBLE,y)
#define CONSTRUCT_SIMPLE_CHAR(x,y)     CONSTRUCT_SIMPLE(x,ST_CHAR,y)
#define CONSTRUCT_SIMPLE_WIDECHAR(x,y) CONSTRUCT_SIMPLE(x,ST_WIDECHAR,y)


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type array T_DATA_OBJ::SIMPLE
 \param  w the address will be casted to (void *)!
 \param  x simple type like ST_U8, ST_I16 ....
 \param  y array-length
 \param  z storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_ARRAY_SIMPLE(w,x,y,z) {#w, (void *)(w),y,(SIZE_##x)*(y),z,ARRAY, \
                                          (void  const*)(x), &cgClassInitializer_SIMPLE }
//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type array T_DATA_OBJ::SIMPLE
 \param  x the address will be casted to (void *)!
 \param  y array-length
 \param  z storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_ARRAY_SIMPLE_U8(x,y,z)       CONSTRUCT_ARRAY_SIMPLE(x,ST_U8,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_U16(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_U16,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_U32(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_U32,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_I32(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_I32,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_T32(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_T32,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_U64(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_U64,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_I64(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_I64,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_BIT16(x,y)      CONSTRUCT_ARRAY_SIMPLE(x,ST_BIT16,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_I16(x,y,z)      CONSTRUCT_ARRAY_SIMPLE(x,ST_I16,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_E8(x,y,z)       CONSTRUCT_ARRAY_SIMPLE(x,ST_E8,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_BIT8(x,y,z)     CONSTRUCT_ARRAY_SIMPLE(x,ST_BIT8,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_FLOAT(x,y,z)    CONSTRUCT_ARRAY_SIMPLE(x,ST_FLOAT,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_DOUBLE(x,y,z)   CONSTRUCT_ARRAY_SIMPLE(x,ST_DOUBLE,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_CHAR(x,y,z)     CONSTRUCT_ARRAY_SIMPLE(x,ST_CHAR,y,z)
#define CONSTRUCT_ARRAY_SIMPLE_WIDECHAR(x,y,z) CONSTRUCT_ARRAY_SIMPLE(x,ST_WIDECHAR,y,z)



//----------------------------------------------------------------------------------------------------------
//! prototypes of the method implementations of T_DATA_OBJ::SIMPLE


//--------------------------------------------------------------------------------------------------
/*!
 \brief  Reads the addressed attribute range-information
 \param  me; pointer of T_DATA_OBJ
 \param  ptrDescriptor range-information
 \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
 \return error-code
 <pre>
     OK                    operation was successful
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
 </pre>
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetAttributeDescription_SIMPLE(const T_DATA_OBJ *me,
                               T_DO_ATTRIB_RANGE * ptrDescriptor,
                               uint16_t attributeIndex);

//--------------------------------------------------------------------------------------------------
/*!
 \brief  Read object description
 \param  me; pointer of T_DATA_OBJ
 \param  ptrDescriptor object description
 \return error-code
 <pre>
     OK                    operation was successful
 </pre>
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetObjectDescription_SIMPLE(const struct _T_DATA_OBJ *me,
                                  T_DO_OBJECT_DESCRIPTION * ptrDescriptor);




#endif /* T_UNIT_T_DATA_OBJ_SIMPLE_H_ */
