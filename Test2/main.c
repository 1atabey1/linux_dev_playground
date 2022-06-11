/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2019 SEGGER Microcontroller GmbH                    *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Semihosting test application.

*/

#include "SEGGER_SEMIHOST.h"

#include <stdio.h>
#include <string.h>

#define _FILENAME_OPEN    "c:\\temp\\test_semihosting.txt"
#define _FILENAME_RENAME  "c:\\temp\\test_semihosting_renamed.txt"
#define _FILENAME_REMOVE  "c:\\temp\\test_semihosting_removed.txt"

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Test_SYS_OPEN()
*
*  Function description
*    Test opening a file on the host system.
*
*  Return value
*     = -1: Failed to open the file.
*    != -1: Handle to the file on the host system.
*/
static int _Test_SYS_OPEN(void) {
  int         hFile;

  SEGGER_SEMIHOST_Write0("Opening file on host system (SYS_OPEN)):\n");
  SEGGER_SEMIHOST_Write0(_FILENAME_OPEN);
  SEGGER_SEMIHOST_Write0("\n");
  hFile = SEGGER_SEMIHOST_Open(_FILENAME_OPEN, SYS_FILE_MODE_WRITEREADBINARY, strlen(_FILENAME_OPEN));
  if (hFile == -1) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");

  return hFile;

}

/*********************************************************************
*
*       _Test_SYS_WRITE0()
*
*  Function description
*    Test writing a string to the host terminal.
*/
static void _Test_SYS_WRITE0(void) {
  SEGGER_SEMIHOST_Write0("Writing 0-terminated string (SYS_WRITE0):\n");
  SEGGER_SEMIHOST_Write0("OK\n");
  SEGGER_SEMIHOST_Write0("=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_WRITE()
*
*  Function description
*    Test writing a string array to the host terminal.
*/
static void _Test_SYS_WRITE(void) {
  char* sMsg;

  sMsg = "Writing string (SYS_WRITE):\n";
  SEGGER_SEMIHOST_Write((int)stdout, sMsg, strlen(sMsg));
  sMsg = "This string is written to errout\n";
  SEGGER_SEMIHOST_Write((int)stderr, sMsg, strlen(sMsg));
  SEGGER_SEMIHOST_Write0("OK\n");
  SEGGER_SEMIHOST_Write0("=============================\n");

}

/*********************************************************************
*
*       _Test_SYS_WRITEC()
*
*  Function description
*    Test writing characters to the host terminal.
*/
static void _Test_SYS_WRITEC(void) {
  char* sMsg;
  char c;

  sMsg = "Writing this string character by character (SYS_WRITEC).\n";
  do {
    c = *sMsg++;
    if (c == 0) {
      break;
    }
    SEGGER_SEMIHOST_WriteC(c);
  } while (1);
  SEGGER_SEMIHOST_Write0("OK\n");
  SEGGER_SEMIHOST_Write0("=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_WRITEF()
*
*  Function description
*    Test printing a host formatted string on the terminal.
*/
static void _Test_SYS_WRITEF(const char* sFmt, ...) {
  va_list List;
  int     r;

  va_start(List, sFmt);

  SEGGER_SEMIHOST_Write0("Writing host-formatted string (SYS_WRITEF):\n");
  r =SEGGER_SEMIHOST_Writef(sFmt, &List);
  if (r < 0) {
    SEGGER_SEMIHOST_Write0("Failed.\n");
  } else {
    SEGGER_SEMIHOST_Write0("OK\n");
  }
  SEGGER_SEMIHOST_Write0("=============================\n");

  va_end(List);
}

/*********************************************************************
*
*       _Test_SYS_WRITE_ToFile()
*
*  Function description
*    Test writing to a file on the host system.
*
*  Parameters
*    hFile: File handle to the opened file, returned by SYS_OPEN.
*/
static void _Test_SYS_WRITE_ToFile(int hFile) {
  char* sMsg;

  sMsg = "Hello Segger!";  // Set message
  SEGGER_SEMIHOST_Write0("Writing message to file (SYS_WRITE):\n");
  SEGGER_SEMIHOST_Write0(sMsg);
  SEGGER_SEMIHOST_Write0("\n");
  if (SEGGER_SEMIHOST_Write(hFile, sMsg, strlen(sMsg)) != 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");

}


/*********************************************************************
*
*       _Test_SYS_READC()
*
*  Function description
*    Test reading characters from user input.
*/
static void _Test_SYS_READC(void) {
  int  r;
  char c;

  SEGGER_SEMIHOST_Write0("Reading character from user input (SYS_READC):\n");
  SEGGER_SEMIHOST_Write0("Enter a character in the Terminal and press enter.\n");
  SEGGER_SEMIHOST_Write0("Reading until 'c' is received.\n");
  do {
    r = SEGGER_SEMIHOST_ReadC();
    if (r < 0) {
      SEGGER_SEMIHOST_Write0("\nFailed");
      break;
    }
    c = (char)r;
    SEGGER_SEMIHOST_WriteC(c);
  } while (c != 'c');
  if (c == 'c') {
    SEGGER_SEMIHOST_Write0("\nOK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_READ()
*
*  Function description
*    Test reading a string from user input.
*/
static void _Test_SYS_READ(void) {
  int   r;
  char  ac[32];

  SEGGER_SEMIHOST_Write0("Reading characters from user input (SYS_READ):\n");
  SEGGER_SEMIHOST_Write0("Enter \"Hello SEGGER\" and send it.\n");
  memset(ac, 0, sizeof(ac));
  r = SEGGER_SEMIHOST_Read((int)stdin, ac, sizeof(ac));
  if (r < 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    if (strcmp(ac, "Hello SEGGER") == 0) {
        SEGGER_SEMIHOST_Write0("\nOK");
    } else {
      SEGGER_SEMIHOST_Write0("Failed!\nExpected \"Hello SEGGER\"\ngot: ");
      SEGGER_SEMIHOST_Write((int)stdout, ac, sizeof(ac) - r);
    }
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_READ_FromFile()
*
*  Function description
*    Test reading from a file on the host system.
*
*  Parameters
*    hFile: File handle to the opened file, returned by SYS_OPEN.
*/
static void _Test_SYS_READ_FromFile(int hFile) {
  int  r;
  char ac[16];

  SEGGER_SEMIHOST_Write0("Reading from file (SYS_READ):\n");
  memset(ac, 0, sizeof(ac));
  r = SEGGER_SEMIHOST_Read(hFile, ac, sizeof(ac));
  if (r < 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else if (r == sizeof(ac)) {
    SEGGER_SEMIHOST_Write0("Failed. Read EOF");
  } else {
    SEGGER_SEMIHOST_Write((int)stdout, ac, sizeof(ac) - r);
    SEGGER_SEMIHOST_Write0("\n");
    snprintf(ac, sizeof(ac), "Read %d bytes\n", sizeof(ac) - r);
    SEGGER_SEMIHOST_Write0(ac);
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_FLEN()
*
*  Function description
*    Test getting the length of a file on the host system.
*
*  Parameters
*    hFile: File handle to the opened file, returned by SYS_OPEN.
*/
static void _Test_SYS_FLEN(int hFile) {
  int   r;
  char  ac[16];

  SEGGER_SEMIHOST_Write0("Getting length of the file (SYS_FLEN):\n");
  r = SEGGER_SEMIHOST_FLen(hFile);
  if (r >= 0) {
    snprintf(ac, sizeof(ac), "Filesize: %d\n", r);
    SEGGER_SEMIHOST_Write0(ac);
    SEGGER_SEMIHOST_Write0("OK");
  } else {
    SEGGER_SEMIHOST_Write0("Failed");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_SEEK()
*
*  Function description
*    Test setting the cursor in a file on the host system.
*
*  Parameters
*    hFile: File handle to the opened file, returned by SYS_OPEN.
*/
static void _Test_SYS_SEEK(int hFile) {
  int r;

  SEGGER_SEMIHOST_Write0("Setting file cursor to position 6 (SYS_SEEK):\n");
  r = SEGGER_SEMIHOST_Seek(hFile, 6);
  if (r != 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_CLOSE()
*
*  Function description
*    Test closing a file on the host system.
*
*  Parameters
*    hFile: File handle to the opened file, returned by SYS_OPEN.
*/
static void _Test_SYS_CLOSE(int hFile) {
  SEGGER_SEMIHOST_Write0("Closing file (SYS_CLOSE):\n");
  if (SEGGER_SEMIHOST_Close(hFile) != 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_RENAME()
*
*  Function description
*    Test renaming a file on the host system.
*/
static void _Test_SYS_RENAME(void) {
  int r;

  SEGGER_SEMIHOST_Write0("Renaming file (SYS_RENAME):\n");
  SEGGER_SEMIHOST_Write0(_FILENAME_OPEN);
  SEGGER_SEMIHOST_Write0(" to ");
  SEGGER_SEMIHOST_Write0(_FILENAME_RENAME);
  SEGGER_SEMIHOST_Write0("\n");
  
  r = SEGGER_SEMIHOST_Rename(_FILENAME_OPEN, strlen(_FILENAME_OPEN), _FILENAME_RENAME, strlen(_FILENAME_RENAME));
  if (r != 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       _Test_SYS_REMOVE()
*
*  Function description
*    Test removing a file on the host system.
*/
static void _Test_SYS_REMOVE(void) {
  int hFile;
  int r;

  SEGGER_SEMIHOST_Write0("Removing file (SYS_REMOVE):\n");
  SEGGER_SEMIHOST_Write0("Creating and removing ");
  SEGGER_SEMIHOST_Write0(_FILENAME_REMOVE);
  SEGGER_SEMIHOST_Write0("\n");

  hFile = SEGGER_SEMIHOST_Open(_FILENAME_REMOVE, SYS_FILE_MODE_WRITEBINARY, strlen(_FILENAME_REMOVE));
  if (hFile == -1) {
    SEGGER_SEMIHOST_Write0("Failed to create file");
  } else {
    SEGGER_SEMIHOST_Close(hFile);
  }

  r = SEGGER_SEMIHOST_Remove(_FILENAME_REMOVE, strlen(_FILENAME_REMOVE));
  if (r != 0) {
    SEGGER_SEMIHOST_Write0("Failed");
  } else {
    SEGGER_SEMIHOST_Write0("OK");
  }
  SEGGER_SEMIHOST_Write0("\n=============================\n");
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       main()
*/
int main(int argc, char* argv[]) {
  int   hFile;

  (void)argc;   // Avoid unused parameter warning
  (void)argv;   // Avoid unused parameter warning

  SEGGER_SEMIHOST_Write0("Semihosting tests:\n");
  SEGGER_SEMIHOST_Write0("\n=============================\n");
  //
  // Run file tests
  //
  hFile = _Test_SYS_OPEN();
  if (hFile != -1) {
    _Test_SYS_WRITE_ToFile(hFile);
    _Test_SYS_FLEN(hFile);
    _Test_SYS_SEEK(hFile);
    _Test_SYS_READ_FromFile(hFile);
    _Test_SYS_CLOSE(hFile);
    _Test_SYS_RENAME();
    _Test_SYS_REMOVE();
  }
  //
  // Run console I/O tests
  //
  _Test_SYS_WRITE0();
  _Test_SYS_WRITE();
  _Test_SYS_WRITEC();
  _Test_SYS_READ();
  _Test_SYS_READC();
  //
  // Run printf tests
  //
  _Test_SYS_WRITEF("Some format string %d...", 1209);
  
  SEGGER_SEMIHOST_Write0("\n=============================\n");
  SEGGER_SEMIHOST_Write0("Semihosting tests done.\n");
  //
  // All tests done, exit from application
  //
  SEGGER_SEMIHOST_Exit(0);

  while (1);

  return 0;
}

/*************************** End of file ****************************/
