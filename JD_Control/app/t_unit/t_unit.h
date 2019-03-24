/*
 * t_unit.h
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#ifndef SRC_T_UNIT_H_
#define SRC_T_UNIT_H_


#pragma GCC diagnostic ignored "-Wpadded"

#define OBJ_DATA_LEN		50
typedef struct
{
	uint16_t 	type;
	uint16_t 	len;
	uint8_t		data[OBJ_DATA_LEN];
}OBJ_DATA;

enum
{
	U8 = 0,
	I8,
	ASCII,
	U16,
	I16,
	U32,
	I32,
	FLOAT,
	DOUBLE,
	TYPE_MX,
};




//----------------------------------------------------------------------------------------------------------
//! states of a T_UNIT subsystem
enum E_UNIT_STATES
{
    NOT_INITIALIZED=0,   //!< the subsystem is not initialized--> resource semaphore not initialized!!

    INITIALIZED=0x40,    //!< the subsystem is initialized
    INACTIVE   =0x40,    //!< the subsystem is initialized and calculation is inactive
    PREPARE    =0x41,    //!< the subsystem is initialized and preparing the calculation
    ACTIVE     =0x42,    //!< the subsystem is initialized and calculation is active
    SIMULATION =0x43,    //!< the subsystem is initialized, calculating is active and output=simulation-value

    ERROR_STATE=0x80     //!< the subsystem can't work, because of an error, but it is still initialized!!
};

//----------------------------------------------------------------------------------------------------------
//! return-codes of a T_UNIT subsystem
//  T_UNIT uses the return codes defined by T_DATA_OBJ

//----------------------------------------------------------------------------------------------------------
//! initialize-classes of subsystems
enum E_UNIT_INITIALIZE_CLASSES
{
    INIT_HARDWARE   =0x80, //!< initialize hardware used by the subsystem;
    INIT_TASKS      =0x40, //!< initialize tasks, semaphores, timer ....;
    INIT_DATA       =0x20, //!< initialize data and data structures
    INIT_CALCULATION=0x10  /*!< if a subsystem needs onle once a preparation for calculating, then
                                use this instead of the state PREPARE */
};

//! class data-class
typedef struct _T_DATACLASS
{
    uint16_t length;
    const void * ptrInitializer;
    union
    {
        const void * dcConstant;
              void * dcDynamic;
              void * dcStatic;
    } ptrData;
    const void * ptrDefault;

    uint16_t (*LoadRomDefaults)(const struct _T_DATACLASS *me);
} T_DATACLASS;

#define ALL_DATACLASSES -1


//----------------------------------------------------------------------------------------------------------
//!  definition of class T_UNIT
typedef struct _T_UNIT
{
    // privat attributes
	uint8_t*  t_unit_name;
    uint16_t * ptrState;
    OS_RSEMA*  ptrDataSemaphore;

    const T_DATA_OBJ * ptrObjectList;
    uint32_t               maxIdx;

    const T_DATACLASS * ptrDataClasses;
    const uint32_t          numOfDataClasses;

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  initialize the subsystem; (hardware, tasks, data, calculation)
     <pre>
        Without INIT_TASKS no T_UNIT-Method will run, cause the semaphore must be initialized!
        After INIT_TASKS the Subsystem is INITIALIZED and INACTIVE!
        It depends to the subsystem design, if the other kinds of initialization are supported or not.
        It depends also to the subsystem-design, if the state will change immediately to ACTIVE.
        Usually the execute-method should handle the Unit-State.
        Example: LowPass function  out = (lastOut*Tlowpass+in*Tsample)/(Tlowpass+Tsample)

            - initialize set state to INACTIVE --> execute() could be scheduled but nothing happens
                                                   the state of the execute() output is NOT_VALID

            - ChangeUnitState set state to PREPARE --> next time execute() is scheduled the filter will be
                                                       loaded and the state changes to active
                                                       the state of the execute() output is NOT_VALID

            - next schedule of execute() -->  the state of the execute() output is VALID

     </pre>
      \param  typeOfStartUp
     <pre>
       INIT_HARDWARE   =0x80  initialize hardware used by the subsystem
       INIT_TASKS      =0x40  initialize tasks, semaphores, timer ....
       INIT_DATA       =0x20  initialize data and data structures
       INIT_CALCULATION=0x10  if a subsystem needs onle once a preparation for calculating, then
     </pre>
     \return error-code
     <pre>
       OK                      operation was successful
       ERROR_NV_STORAGE        attributes inside the nv-storage are corrupted
       ERROR_RAM_STORAGE       attributes inseid the ram are corrupted
       ERROR_HW                hardware used by the subsystem doesn't work
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*Initialize)(const struct _T_UNIT *me, uint8_t typeOfStartUp);

    //--------------------------------------------------------------------------------------------------
    // Dataclass Methods
    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  That method calls LoadRomDefaults of the dataClass[dataClassIndex]
     <pre>
        in mode "NOT_INITIALIZED" the mode will not be changed.
        for all other modes the state will be switched to "INACTIVE" or "ERROR_STATE"
     </pre>

     \param  dataClassIndex
     <pre>
        -1 > dataClassIndex >=numOfDataClasses --> ILLEGAL_DATACLASS_INDEX
        -1 --> all data-classes of the subsystem will be called
        0 - numOfDataClasses-1 --> dataClass[dataClassIndex] will be called
     </pre>
     \return error-code
     <pre>
       OK                       operation was successful
       DECLINE                  DEBUG, operation isn't permitted in the aktual unit-state
       ILLEGAL_DATACLASS_INDEX  -1 > dataClassIndex >=numOfDataClasses
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*LoadRomDefaults)(const struct _T_UNIT *me, int16_t dataClassIndex);

    //--------------------------------------------------------------------------------------------------
    // data-access methods
    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  gets attributeIndex of the data-object objectIndex; only in "INITIALIZED"
     \param  objectIndex    object index
     \param  attributeIndex attribute index; attributeIndex<0 --> get whole ojbject
     \param  ptrValue  pointer for return-Value
     \return error-code
     <pre>
       OK                      operation was successful
       DECLINE                 DEBUG, operation isn't permitted in the aktual unit-state
       ILLEGAL_OBJ_IDX         DEBUG, unknown Object
       ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*Get)(const struct _T_UNIT *me,
                    uint16_t objectIndex,
                    int16_t   attributeIndex,
                    void * ptrValue);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  put the attribute attributeIndex of the data-object objectIndex ; only in "INITIALIZED"
     \param  objectIndex    object index
     \param  attributeIndex attribute index; <0 --> put whole object
     \param  ptrValue  pointer to attribute-value and return-Value
     \return error-code
     <pre>
       OK                      operation was successful
       LESSTHAN_RANGEMIN       at least one value is less than its minimum value
       GREATERTHAN_RANGEMAX    at least one value is greater than its maximum
       SPAN_TO_SMALL           then span between two values is to small
       NOT_ON_GRID             at least one value is not a multiple of its increment
       RULE_VIOLATION          this shows a violation of one or more businessrules
       DECLINE                 DEBUG, operation isn't permitted in the aktual unit-state
       ILLEGAL_OBJ_IDX         DEBUG, unknown Object
       ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*Put)(const struct _T_UNIT *me,
                    uint16_t objectIndex,
                    int16_t   attributeIndex,
                    void * ptrValue);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  check value against the attribute/object business-rules without influence to the
             attribute/ojectt ; only in "INITIALIZED"
     \param  objectIndex    object index
     \param  attributeIndex attribute index; <0 --> put whole object
     \param  ptrValue  pointer to attribute-value and return-Value
     \return error-code
     <pre>
       OK                      operation was successful
       LESSTHAN_RANGEMIN       at least one value is less than its minimum value
       GREATERTHAN_RANGEMAX    at least one value is greater than its maximum
       SPAN_TO_SMALL           then span between two values is to small
       NOT_ON_GRID             at least one value is not a multiple of its increment
       RULE_VIOLATION          this shows a violation of one or more businessrules
       DECLINE                 DEBUG, operation isn't permitted in the aktual unit-state
       ILLEGAL_OBJ_IDX         DEBUG, unknown Object
       ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*Check)(const struct _T_UNIT *me,
                      uint16_t objectIndex,
                      int16_t   attributeIndex,
                      void * ptrValue);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  Get the description of the addressed attribute
     \param  ptrDescriptor range-information
       <pre>
              ptrDescriptor->rulesAreActive ==eFALSE--> this attribute will not be validated --> no range-info
              ptrDescriptor->dataType       :  type info for the following union
              ptrDescriptor->range          : (union) Conteiner for range-information

              example for simple type ST_INT16 (refer to simple_type.h unit T_DATA_OBJ)

                ptrDescriptor->range.i16.min       : minimum
                ptrDescriptor->range.i16.max       : maximum
                ptrDescriptor->range.i16.increment : increment=5 --> data value must be a multiple of 5

              for ST_E8 (tabled enumeration)
                ptrDescriptor->range.e8.numberOfCodes                  : as the name says
                ptrDescriptor->range.e8.aryCodeTab[0..numberOfCodes-1] :  ""
       </pre>
     \param  attributeIndex ==-1 --> putObject; >=0 --> putAttribute
     \return error-code
     <pre>
         OK                    operation was successful
         ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*GetAttributeDescription)(const struct _T_UNIT *me,
                                        uint16_t objectIndex,
                                        uint16_t attributeIndex,
                                        T_DO_ATTRIB_RANGE * ptrDescriptor);

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief  get information about the addressed object
     \param  the object description will be returned in ptrObjDescriptor
       <pre>
         ptrDescriptor->objectLength         : byte length of the whole object
         ptrDescriptor->numberOfAttributes   : SIMPLE -> 1
                                               ARRAY  -> ARRAY-length
                                               STRUCT -> number of struct member
         ptrDescriptor->aryAttribDataTypes[] : for SIMPLE and ARRAY only [0];
                                               for STRUCT [0-numberOfAttributes-1]
         ptrDescriptor->typeQualifier        : SIMPLE, ARRAY, STRUCT
       </pre>
     \param  ==0 --> object-info else error
     \return error-code
     <pre>
         OK                    operation was successful
     </pre>
    */
    //--------------------------------------------------------------------------------------------------
    uint16_t (*GetObjectDescription)(const struct _T_UNIT *me,
                                     uint16_t objectIndex,
                                     T_DO_OBJECT_DESCRIPTION * ptrDescriptor);

    uint16_t (*GetObjectName)(const struct _T_UNIT *me,
							 uint16_t objectIndex,
							 uint8_t * ptrDescriptor,
							 uint16_t* ptrLength);

} T_UNIT;




#pragma GCC diagnostic pop
//----------------------------------------------------------------------------------------------------------
// prototypes of the T_UNIT methods
//----------------------------------------------------------------------------------------------------------


uint16_t Initialize_T_UNIT(const T_UNIT *me, uint8_t typeOfStartUp);

uint16_t LoadRomDefaults_T_UNIT(const T_UNIT *me, int16_t dataClassIndex);

uint16_t Get_T_UNIT(const T_UNIT *me,
                    uint16_t objectIndex,
                    int16_t attributeIndex,
                    void * ptrValue);

uint16_t Put_T_UNIT(const T_UNIT *me,
                    uint16_t objectIndex,
                    int16_t attributeIndex,
                    void * ptrValue);

uint16_t Check_T_UNIT(const T_UNIT *me,
                      uint16_t objectIndex,
                      int16_t attributeIndex,
                      void * ptrValue);


uint16_t GetAttributeDescription_T_UNIT(const T_UNIT *me,
                                        uint16_t objectIndex,
                                        uint16_t attributeIndex,
                                        T_DO_ATTRIB_RANGE * ptrDescriptor);


uint16_t GetObjectDescription_T_UNIT(const T_UNIT *me,
                                     uint16_t objectIndex,
                                     T_DO_OBJECT_DESCRIPTION * ptrDescriptor);

uint16_t GetObjectName_T_UNIT(const T_UNIT *me,
                                     uint16_t objectIndex,
                                     uint8_t * ptrDescriptor,
									 uint16_t* ptrLength);
#endif /* SRC_T_UNIT_H_ */
