/*
 * t_data_obj_const.h
 *
 *  Created on: 2016��11��1��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_CONST_H_
#define T_UNIT_T_DATA_OBJ_CONST_H_




//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ_USIGN8
extern const T_DATA_CLASS cgClassInitializer_CONST;

//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::CONST
 \param  w ptrValue (const void SLOW*)
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_CONST(w) {#w, 0,1,1,ROM,SIMPLE, w, &cgClassInitializer_CONST }
#define CONSTRUCT_CONST_U8(w) {#w, 0,1,1,ROM,SIMPLE, w, &cgClassInitializer_CONST }
//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type array T_DATA_OBJ:CONST
 \param  w ptrValue (const void SLOW*)
 \param  x array-length
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_ARRAY_CONST(w,x) {#w, 0,x,x,ROM,ARRAY, w, &cgClassInitializer_CONST }
#define CONSTRUCT_ARRAY_CONST_U8(w,x) {#w, 0,x,x,ROM,ARRAY, w, &cgClassInitializer_CONST }




//--------------------------------------------------------------------------------------------------
/*!
 \brief  Reads the addressed attribute or the whole object out of ROM or RAM.
         This method doesn't support objects resided in non-volatile areas!

 \param  ptrValue the new object or attribute value
 \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
 \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
 \return error-code
 <pre>
     OK                    operation was successful
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
     METHOD_NOT_SUPPORTED  RAM and NON_VOLATILE are not supported
 </pre>
 \warning
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Get_CONST(const T_DATA_OBJ *me,
                   void * ptrValue,
                   int16_t attributeIndex,
				   OS_RSEMA * ptrDataSemaphore);



#endif /* T_UNIT_T_DATA_OBJ_CONST_H_ */
