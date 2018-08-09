/**************************************************************************//*****
 * @file     printf.c
 * @brief    Implementation of several stdio.h methods, such as printf(), 
 *           sprintf() and so on. This reduces the memory footprint of the
 *           binary when using those methods, compared to the libc implementation.
 ********************************************************************************/
#include <stdio.h>
#include <stdarg.h>

#include <stm32f4xx.h>

static void InitUSART2(int speed);

static void InitUSART2(int speed)
{
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

	speed = speed;
	/* Enable clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Configure UART2_Tx pin PA2 */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init (GPIOA, &GPIO_InitStructure);

	/* Connect USART2 pins to AF2 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	/* Configure UART2_Rx pin PA3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init (GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx);

	USART_Init(USART2, &USART_InitStructure);
	/* enable USART2 */
	USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  Transmit a char, if you want to use printf(), 
 *         you need implement this function
 *
 * @param  pStr	Storage string.
 * @param  c    Character to write.
 */
void PrintChar(char c)
{
static uint8_t fInit = 0;

		if (fInit == 0) {
			/* HW Init */
			InitUSART2(115200);
			fInit = 1;
		}

		if (c == '\n') {
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			{
				;
			}

			USART_SendData(USART2, (uint8_t) '\r');
		}


		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		{
			;
		}

		/* Place your implementation of fputc here */
		/* e.g. write a character to the USART */
		USART_SendData(USART2, (uint8_t) c);


	  return;
}

/** Maximum string size allowed (in bytes). */
#define MAX_STRING_SIZE         100


/** Required for proper compilation. */
struct _reent r = {0, (FILE *) 0, (FILE *) 1, (FILE *) 0};
struct _reent *_impure_ptr = &r;

/**
 * @brief  Writes a character inside the given string. Returns 1.
 *
 * @param  pStr	Storage string.
 * @param  c    Character to write.
 */
signed int PutChar(char *pStr, char c)
{
    *pStr = c;
    return 1;
}


/**
 * @brief  Writes a string inside the given string.
 *
 * @param  pStr     Storage string.
 * @param  pSource  Source string.
 * @return  The size of the written
 */
signed int PutString(char *pStr, const char *pSource)
{
    signed int num = 0;

    while (*pSource != 0) {

        *pStr++ = *pSource++;
        num++;
    }

    return num;
}


/**
 * @brief  Writes an unsigned int inside the given string, using the provided fill &
 *         width parameters.
 *
 * @param  pStr  Storage string.
 * @param  fill  Fill character.
 * @param  width  Minimum integer width.
 * @param  value  Integer value.   
 */
signed int PutUnsignedInt(
    char *pStr,
    char fill,
    signed int width,
    unsigned int value)
{
    signed int num = 0;

    /* Take current digit into account when calculating width */
    width--;

    /* Recursively write upper digits */
    if ((value / 10) > 0) {

        num = PutUnsignedInt(pStr, fill, width, value / 10);
        pStr += num;
    }
    
    /* Write filler characters */
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    /* Write lower digit */
    num += PutChar(pStr, (value % 10) + '0');

    return num;
}


/**
 * @brief  Writes a signed int inside the given string, using the provided fill & width
 *         parameters.
 *
 * @param pStr   Storage string.
 * @param fill   Fill character.
 * @param width  Minimum integer width.
 * @param value  Signed integer value.
 */
signed int PutSignedInt(
    char *pStr,
    char fill,
    signed int width,
    signed int value)
{
    signed int num = 0;
    unsigned int absolute;

    /* Compute absolute value */
    if (value < 0) {

        absolute = -value;
    }
    else {

        absolute = value;
    }

    /* Take current digit into account when calculating width */
    width--;

    /* Recursively write upper digits */
    if ((absolute / 10) > 0) {

        if (value < 0) {
        
            num = PutSignedInt(pStr, fill, width, -(absolute / 10));
        }
        else {

            num = PutSignedInt(pStr, fill, width, absolute / 10);
        }
        pStr += num;
    }
    else {

        /* Reserve space for sign */
        if (value < 0) {

            width--;
        }

        /* Write filler characters */
        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }

        /* Write sign */
        if (value < 0) {

            num += PutChar(pStr, '-');
            pStr++;
        }
    }

    /* Write lower digit */
    num += PutChar(pStr, (absolute % 10) + '0');

    return num;
}


/**
 * @brief  Writes an hexadecimal value into a string, using the given fill, width &
 *         capital parameters.
 *
 * @param pStr   Storage string.
 * @param fill   Fill character.
 * @param width  Minimum integer width.
 * @param maj    Indicates if the letters must be printed in lower- or upper-case.
 * @param value  Hexadecimal value.
 *
 * @return  The number of char written
 */
signed int PutHexa(
    char *pStr,
    char fill,
    signed int width,
    unsigned char maj,
    unsigned int value)
{
    signed int num = 0;

    /* Decrement width */
    width--;

    /* Recursively output upper digits */
    if ((value >> 4) > 0) {

        num += PutHexa(pStr, fill, width, maj, value >> 4);
        pStr += num;
    }
    /* Write filler chars */
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    /* Write current digit */
    if ((value & 0xF) < 10) {

        PutChar(pStr, (value & 0xF) + '0');
    }
    else if (maj) {

        PutChar(pStr, (value & 0xF) - 10 + 'A');
    }
    else {

        PutChar(pStr, (value & 0xF) - 10 + 'a');
    }
    num++;

    return num;
}



/* Global Functions ----------------------------------------------------------- */


/**
 * @brief  Stores the result of a formatted string into another string. Format
 *         arguments are given in a va_list instance.
 *
 * @param pStr    Destination string.
 * @param length  Length of Destination string.
 * @param pFormat Format string.
 * @param ap      Argument list.
 *
 * @return  The number of characters written.
 */
signed int vsnprintf(char *pStr, size_t length, const char *pFormat, va_list ap)
{
    char          fill;
    unsigned char width;
    signed int    num = 0;
    signed int    size = 0;

    /* Clear the string */
    if (pStr) {

        *pStr = 0;
    }

    /* Phase string */
    while (*pFormat != 0 && size < length) {

        /* Normal character */
        if (*pFormat != '%') {

            *pStr++ = *pFormat++;
            size++;
        }
        /* Escaped '%' */
        else if (*(pFormat+1) == '%') {

            *pStr++ = '%';
            pFormat += 2;
            size++;
        }
        /* Token delimiter */
        else {

            fill = ' ';
            width = 0;
            pFormat++;

            /* Parse filler */
            if (*pFormat == '0') {

                fill = '0';
                pFormat++;
            }

            /* Parse width */
            while ((*pFormat >= '0') && (*pFormat <= '9')) {
        
                width = (width*10) + *pFormat-'0';
                pFormat++;
            }

            /* Check if there is enough space */
            if (size + width > length) {

                width = length - size;
            }

            /* ignore long Format / same size than int */
            if(*pFormat == 'l') {
            	pFormat++;
            }
        
            /* Parse type */
            switch (*pFormat) {
            case 'd': 
            case 'i': num = PutSignedInt(pStr, fill, width, va_arg(ap, signed int)); break;
            case 'u': num = PutUnsignedInt(pStr, fill, width, va_arg(ap, unsigned int)); break;
            case 'x': num = PutHexa(pStr, fill, width, 0, va_arg(ap, unsigned int)); break;
            case 'X': num = PutHexa(pStr, fill, width, 1, va_arg(ap, unsigned int)); break;
            case 's': num = PutString(pStr, va_arg(ap, char *)); break;
            case 'c': num = PutChar(pStr, va_arg(ap, unsigned int)); break;
            default:
                return EOF;
            }

            pFormat++;
            pStr += num;
            size += num;
        }
    }

    /* NULL-terminated (final \0 is not counted) */
    if (size < length) {

        *pStr = 0;
    }
    else {

        *(--pStr) = 0;
        size--;
    }

    return size;
}


/**
 * @brief  Stores the result of a formatted string into another string. Format
 *         arguments are given in a va_list instance.
 *
 * @param pStr    Destination string.
 * @param length  Length of Destination string.
 * @param pFormat Format string.
 * @param ...     Other arguments
 *
 * @return  The number of characters written.
 */
signed int snprintf(char *pString, size_t length, const char *pFormat, ...)
{
    va_list    ap;
    signed int rc;

    va_start(ap, pFormat);
    rc = vsnprintf(pString, length, pFormat, ap);
    va_end(ap);

    return rc;
}


/**
 * @brief  Stores the result of a formatted string into another string. Format
 *         arguments are given in a va_list instance.
 *
 * @param pString  Destination string.
 * @param length   Length of Destination string.
 * @param pFormat  Format string.
 * @param ap       Argument list.
 *
 * @return  The number of characters written.
 */
signed int vsprintf(char *pString, const char *pFormat, va_list ap)
{
   return vsnprintf(pString, MAX_STRING_SIZE, pFormat, ap);
}

/**
 * @brief  Outputs a formatted string on the given stream. Format arguments are given
 *         in a va_list instance.
 *
 * @param pStream  Output stream.
 * @param pFormat  Format string
 * @param ap       Argument list. 
 */
signed int vfprintf(FILE *pStream, const char *pFormat, va_list ap)
{
    char pStr[MAX_STRING_SIZE];
    char pError[] = "stdio.c: increase MAX_STRING_SIZE\n\r";

    /* Write formatted string in buffer */
    if (vsprintf(pStr, pFormat, ap) >= MAX_STRING_SIZE) {

        fputs(pError, stderr);
        while (1); /* Increase MAX_STRING_SIZE */
    }

    /* Display string */
    return fputs(pStr, pStream);
}


/**
 * @brief  Outputs a formatted string on the DBGU stream. Format arguments are given
 *         in a va_list instance.
 *
 * @param pFormat  Format string.
 * @param ap  Argument list.
 */
signed int vprintf(const char *pFormat, va_list ap)
{
    return vfprintf(stdout, pFormat, ap);
}


/**
 * @brief  Outputs a formatted string on the given stream, using a variable 
 *         number of arguments.
 *
 * @param pStream  Output stream.
 * @param pFormat  Format string.
 */
signed int fprintf(FILE *pStream, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    /* Forward call to vfprintf */
    va_start(ap, pFormat);
    result = vfprintf(pStream, pFormat, ap);
    va_end(ap);

    return result;
}

#define MAX_PRINT_SIZE	128
static uint8_t printBuff[MAX_PRINT_SIZE];
/**
 * @brief  Outputs a formatted string on the DBGU stream, using a variable number of
 *         arguments.
 *
 * @param  pFormat  Format string.
 */
signed int printf(const char *pFormat, ...)
{
	uint16_t len = 0;
    va_list ap;
    signed int result;

    /* Forward call to vprintf */
    va_start(ap, pFormat);
    len = (uint8_t)vsnprintf((char*)&fprintf[0], MAX_PRINT_SIZE, pFormat,ap);
    va_end(ap);
    HAL_UART_Transmit_DMA(& , (uint8_t*)printBuff, (uint16_t)len);
    return result;
}


/**
 * @brief  Writes a formatted string inside another string.
 *
 * @param pStr     torage string.
 * @param pFormat  Format string.
 */
signed int sprintf(char *pStr, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    // Forward call to vsprintf
    va_start(ap, pFormat);
    result = vsprintf(pStr, pFormat, ap);
    va_end(ap);

    return result;
}


/**
 * @brief  Outputs a string on stdout.
 *
 * @param pStr  String to output. 
 */
signed int puts(const char *pStr)
{
    return fputs(pStr, stdout);
}


/**
 * @brief  Implementation of fputc using the DBGU as the standard output. Required
 *         for printf().
 *
 * @param c        Character to write.
 * @param pStream  Output stream.
 * @param The character written if successful, or -1 if the output stream is
 *        not stdout or stderr.
 */
signed int fputc(signed int c, FILE *pStream)
{
    if ((pStream == stdout) || (pStream == stderr)) {

    	PrintChar(c);

        return c;
    }
    else {

        return EOF;
    }
}


/**
 * @brief  Implementation of fputs using the DBGU as the standard output. Required
 *         for printf().
 *
 * @param pStr     String to write.
 * @param pStream  Output stream.
 *
 * @return  Number of characters written if successful, or -1 if the output
 *          stream is not stdout or stderr.
 */
signed int fputs(const char *pStr, FILE *pStream)
{
    signed int num = 0;

    while (*pStr != 0) {

        if (fputc(*pStr, pStream) == -1) {

            return -1;
        }
        num++;
        pStr++;
    }

    return num;
}

/* --------------------------------- End Of File ------------------------------ */
