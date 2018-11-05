/* user defined HW API functions for CiA 401 profile implementation */


/* digital out */
/* only printf() output */
void byte_out_printfInd(
	UNSIGNED8 port,		/**< selected 8bit port */
	UNSIGNED16 outVal	/**< output value */
	);

/* use process image simulation */
void byte_out_piInd(
	UNSIGNED8 port,		/**< selected 8bit port */
	UNSIGNED16 outVal	/**< output value */
	);


/* digital in */
UNSIGNED8 byte_in_piInd(
	UNSIGNED8 port,		/**< selected 8bit port */
	UNSIGNED8 filter	/** used input filter */
	);

/* analog out */
/* only printf() output */
void analog_out_printfInd(
	UNSIGNED8 port,		/**< selected 16bit analog channel */
	INTEGER16 outVal	/**< output value */
	);

/* use process image simulation */
void analog_out_piInd(
	UNSIGNED8 port,		/**< selected 16bit analog channel */
	INTEGER16 outVal	/**< output value */
	);

/* analog in */
INTEGER16 analog_in_piInd(
	UNSIGNED8 port		/**< selected 16bit analog channel */
	);

