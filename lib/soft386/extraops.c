/*
 * Soft386 386/486 CPU Emulation Library
 * extraops.c
 *
 * Copyright (C) 2013 Aleksandar Andrejevic <theflash AT sdf DOT lonestar DOT org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* INCLUDES *******************************************************************/

// #define WIN32_NO_STATUS
// #define _INC_WINDOWS
#include <windef.h>

// #define NDEBUG
#include <debug.h>

#include <soft386.h>
#include "opcodes.h"
#include "common.h"

/* PUBLIC VARIABLES ***********************************************************/

SOFT386_OPCODE_HANDLER_PROC
Soft386ExtendedHandlers[SOFT386_NUM_OPCODE_HANDLERS] =
{
    NULL, // TODO: OPCODE 0x00 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x01 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x02 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x03 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x04 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x05 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x06 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x07 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x08 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x09 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x0A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x0B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x0C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x0D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x0E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x0F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x10 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x11 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x12 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x13 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x14 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x15 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x16 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x17 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x18 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x19 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x1A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x1B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x1C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x1D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x1E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x1F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x20 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x21 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x22 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x23 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x24 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x25 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x26 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x27 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x28 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x29 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x2A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x2B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x2C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x2D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x2E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x2F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x30 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x31 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x32 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x33 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x34 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x35 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x36 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x37 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x38 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x39 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x3A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x3B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x3C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x3D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x3E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x3F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x40 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x41 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x42 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x43 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x44 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x45 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x46 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x47 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x48 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x49 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x4A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x4B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x4C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x4D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x4E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x4F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x50 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x51 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x52 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x53 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x54 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x55 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x56 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x57 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x58 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x59 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x5A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x5B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x5C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x5D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x5E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x5F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x60 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x61 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x62 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x63 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x64 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x65 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x66 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x67 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x68 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x69 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x6A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x6B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x6C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x6D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x6E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x6F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x70 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x71 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x72 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x73 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x74 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x75 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x76 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x77 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x78 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x79 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x7A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x7B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x7C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x7D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x7E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x7F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x80 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x81 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x82 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x83 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x84 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x85 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x86 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x87 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x88 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x89 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x8A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x8B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x8C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x8D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x8E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x8F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x90 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x91 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x92 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x93 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x94 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x95 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x96 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x97 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x98 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x99 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x9A NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x9B NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x9C NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x9D NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x9E NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0x9F NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA0 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA1 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA2 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA3 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA4 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA5 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA6 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA7 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA8 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xA9 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xAA NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xAB NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xAC NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xAD NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xAE NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xAF NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB0 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB1 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB2 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB3 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB4 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB5 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB6 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB7 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB8 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xB9 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xBA NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xBB NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xBC NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xBD NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xBE NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xBF NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC0 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC1 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC2 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC3 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC4 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC5 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC6 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC7 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC8 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xC9 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xCA NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xCB NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xCC NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xCD NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xCE NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xCF NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD0 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD1 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD2 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD3 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD4 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD5 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD6 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD7 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD8 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xD9 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xDA NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xDB NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xDC NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xDD NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xDE NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xDF NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE0 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE1 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE2 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE3 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE4 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE5 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE6 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE7 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE8 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xE9 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xEA NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xEB NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xEC NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xED NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xEE NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xEF NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF0 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF1 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF2 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF3 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF4 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF5 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF6 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF7 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF8 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xF9 NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xFA NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xFB NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xFC NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xFD NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xFE NOT IMPLEMENTED
    NULL, // TODO: OPCODE 0xFF NOT IMPLEMENTED
};

/* PUBLIC FUNCTIONS ***********************************************************/

SOFT386_OPCODE_HANDLER(Soft386OpcodeExtended)
{
    UCHAR SecondOpcode;

    /* Fetch the second operation code */
    if (!Soft386FetchByte(State, &SecondOpcode))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (Soft386ExtendedHandlers[SecondOpcode] != NULL)
    {
        /* Call the extended opcode handler */
        return Soft386ExtendedHandlers[SecondOpcode](State, SecondOpcode);
    }
    else
    {
        /* This is not a valid opcode */
        Soft386Exception(State, SOFT386_EXCEPTION_UD);
        return FALSE;
    }
}
