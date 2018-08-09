//-------------------------------------------------------------------------------------------------
/*
 <pre>
 Copyright              Copyright ABB, 2007.
                        All rights reserved. Reproduction, modification,
                        use or disclosure to third parties without express
                        authority is forbidden.

 System         Subsystem Parameter

 Module

 Description    This header file defines the complete list of public data-objects.
                Other subsystems need to use these macros instead of magic-numbers  
                when accessing the public data-objects.

                The index of private data-object should not be available through
                the TUNIT interface.

 Remarks
 </pre>
*/
//-------------------------------------------------------------------------------------------------
//@SubGen Version@<0x020101>

#ifdef __PARAMETER_IDX_H__
  #error Parameter_idx.h included more than once
#endif

#define __PARAMETER_IDX_H__

enum _T_PARAMETER_IDX
{
    //@SubGen start@    <ENUMERATIONS>
    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_usbConnectedStatus,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_sd_status,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_usbInit_retry,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_usbWrite_retry,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_usbWriteStatus,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_copy_status,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_copy_filenum,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_totalNum_Copy,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U32
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_resetFileID,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_loadRomDefault,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_saveNV,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_weldCurrent,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_weldWorkPoint,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
	PARA_IDX_weldSpeed,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_weldEnergy,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_inchSpeed,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_inchCurrent,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_shortCount,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_loseArcNum,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_weldMode,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_can_innum,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_can_validatenum,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U32
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_setDifferenceTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8[4]
     \typequalifier     : ARRAY
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_swVersion,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8[4]
     \typequalifier     : ARRAY
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_regData,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8[12]
     \typequalifier     : ARRAY
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_company,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8[8]
     \typequalifier     : ARRAY
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_owner,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U32
     \typequalifier     : SIMPLE
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_deviceID,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U32
     \typequalifier     : SIMPLE
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_factoryTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_deviceType,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U32[6]
     \typequalifier     : ARRAY
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_adjustRTCFactor,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : NON_VOLATILE
     \direction         : CONTAINED
    */
    PARA_IDX_speedMax,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_setMode,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_setCurrent,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_setWorkPoint,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_setWeldSpeed,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_setWeldMask,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : CONST_U8[12]
     \typequalifier     : ARRAY
     \storage qualifier : ROM
     \direction         : CONTAINED
    */
    PARA_IDX_getWireSize,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : CONST_U8[12]
     \typequalifier     : ARRAY
     \storage qualifier : ROM
     \direction         : CONTAINED
    */
    PARA_IDX_getWireType,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : CONST_U8[12]
     \typequalifier     : ARRAY
     \storage qualifier : ROM
     \direction         : CONTAINED
    */
    PARA_IDX_getProcessType,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : CONST_U8[12]
     \typequalifier     : ARRAY
     \storage qualifier : ROM
     \direction         : CONTAINED
    */
    PARA_IDX_getGasType,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : CONST_U8[12]
     \typequalifier     : ARRAY
     \storage qualifier : ROM
     \direction         : CONTAINED
    */
    PARA_IDX_getUnitCode,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_strikeSpeed,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_startTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_strikeTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_upslopeTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_downSlopeTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_craterTime,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_startMoveDelay,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_stopMoveDelay,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     strike speed
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_strikeTrim,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_strikeWorkpoint,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_startTrim,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_startWorkpoint,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_craterTrim,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_craterWorkpoint,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_strikeInch,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_codeInchSpeed,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_weldStatus,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U8
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_InitIIC_Data,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16
     \typequalifier     : SIMPLE
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_IIC_Status,

    //--------------------------------------------------------------------------------------------------
    /*!
     \brief     
     \datatype          : SIMPLE_U16[2]
     \typequalifier     : ARRAY
     \storage qualifier : RAM
     \direction         : CONTAINED
    */
    PARA_IDX_simulateSave,
    //@SubGen end@    <ENUMERATIONS>

	IDX_SYSTEM_RST,
	IDX_FILE_ID,
};




