//---------------------------------------------------------------------------
// NEOPOP : Emulator as in Dreamland
//
// Copyright (c) 2001-2002 by neopop_uk
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

#include "neopop.h"
#include "Z80_interface.h"
#include "gfx.h"
#include "mem.h"
#include "interrupt.h"
#include "sound.h"
#include "dma.h"
#include "bios.h"

//=============================================================================

uint8 ngpc_bios[0x10000];		//Holds bios program data

//=============================================================================

void reset(void)
{
        NGPGfx->power();
	Z80_reset();
	reset_int();
	reset_timers();

	reset_memory();
	BIOSHLE_Reset();
	reset_registers();	// TLCS900H registers
	reset_dma();
}

//=============================================================================

static const uint8 font[0x800] = { 
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0xF8,0xF8,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
	0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
	0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0x20,0x30,0x38,0x3C,0x38,0x30,0x20,0x00,0x04,0x0C,0x1C,0x3C,0x1C,0x0C,0x04,0x00,
	0x00,0x00,0xFE,0x7C,0x38,0x10,0x00,0x00,0x00,0x00,0x10,0x38,0x7C,0xFE,0x00,0x00,
	0x20,0x3C,0x08,0x3C,0x28,0x7E,0x08,0x00,0x3C,0x24,0x3C,0x24,0x3C,0x24,0x24,0x00,
	0x3C,0x24,0x24,0x3C,0x24,0x24,0x3C,0x00,0x10,0x10,0x54,0x54,0x10,0x28,0xC6,0x00,
	0x10,0x12,0xD4,0x58,0x54,0x92,0x10,0x00,0x10,0x10,0x7C,0x10,0x38,0x54,0x92,0x00,
	0x10,0x28,0x7C,0x92,0x38,0x54,0xFE,0x00,0x10,0x10,0x10,0x7C,0x10,0x10,0xFE,0x00,
	0x7F,0xFF,0xE0,0xFF,0x7F,0x01,0xFF,0xFF,0xDC,0xDE,0x1F,0x9F,0xDF,0xDD,0xDC,0x9C,
	0x3B,0x3B,0x3B,0xBB,0xFB,0xFB,0xFB,0x7B,0x8F,0x9E,0xBC,0xF8,0xF8,0xBC,0x9E,0x8F,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x08,0x10,0x00,0x18,0x00,
	0x6C,0x6C,0x24,0x48,0x00,0x00,0x00,0x00,0x14,0x14,0xFE,0x28,0xFE,0x50,0x50,0x00,
	0x10,0x7C,0x90,0x7C,0x12,0xFC,0x10,0x00,0x42,0xA4,0xA8,0x54,0x2A,0x4A,0x84,0x00,
	0x30,0x48,0x38,0x62,0x94,0x88,0x76,0x00,0x18,0x18,0x08,0x10,0x00,0x00,0x00,0x00,
	0x08,0x10,0x20,0x20,0x20,0x10,0x08,0x00,0x20,0x10,0x08,0x08,0x08,0x10,0x20,0x00,
	0x10,0x92,0x54,0x38,0x38,0x54,0x92,0x00,0x10,0x10,0x10,0xFE,0x10,0x10,0x10,0x00,
	0x00,0x00,0x00,0x30,0x30,0x10,0x20,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,
	0x3C,0x42,0x46,0x5A,0x62,0x42,0x3C,0x00,0x08,0x38,0x08,0x08,0x08,0x08,0x08,0x00,
	0x3C,0x42,0x42,0x0C,0x30,0x40,0x7E,0x00,0x3C,0x42,0x02,0x1C,0x02,0x42,0x3C,0x00,
	0x0C,0x14,0x24,0x44,0x7E,0x04,0x04,0x00,0x7E,0x40,0x7C,0x02,0x02,0x42,0x3C,0x00,
	0x3C,0x40,0x7C,0x42,0x42,0x42,0x3C,0x00,0x7E,0x02,0x04,0x08,0x08,0x10,0x10,0x00,
	0x3C,0x42,0x42,0x3C,0x42,0x42,0x3C,0x00,0x3C,0x42,0x42,0x42,0x3E,0x02,0x3C,0x00,
	0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x18,0x08,0x10,0x00,
	0x00,0x08,0x10,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x3C,0x00,0x3C,0x00,0x00,0x00,
	0x00,0x10,0x08,0x04,0x08,0x10,0x00,0x00,0x3C,0x62,0x62,0x0C,0x18,0x00,0x18,0x00,
	0x7C,0x82,0xBA,0xA2,0xBA,0x82,0x7C,0x00,0x10,0x28,0x28,0x44,0x7C,0x82,0x82,0x00,
	0x7C,0x42,0x42,0x7C,0x42,0x42,0x7C,0x00,0x1C,0x22,0x40,0x40,0x40,0x22,0x1C,0x00,
	0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x00,0x7E,0x40,0x40,0x7E,0x40,0x40,0x7E,0x00,
	0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x00,0x3C,0x42,0x80,0x9E,0x82,0x46,0x3A,0x00,
	0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,
	0x02,0x02,0x02,0x02,0x42,0x42,0x3C,0x00,0x42,0x44,0x48,0x50,0x68,0x44,0x42,0x00,
	0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00,0x82,0xC6,0xAA,0x92,0x82,0x82,0x82,0x00,
	0x42,0x62,0x52,0x4A,0x46,0x42,0x42,0x00,0x38,0x44,0x82,0x82,0x82,0x44,0x38,0x00,
	0x7C,0x42,0x42,0x7C,0x40,0x40,0x40,0x00,0x38,0x44,0x82,0x82,0x8A,0x44,0x3A,0x00,
	0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x00,0x3C,0x42,0x40,0x3C,0x02,0x42,0x3C,0x00,
	0xFE,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,
	0x82,0x82,0x44,0x44,0x28,0x28,0x10,0x00,0x82,0x92,0x92,0xAA,0xAA,0x44,0x44,0x00,
	0x82,0x44,0x28,0x10,0x28,0x44,0x82,0x00,0x82,0x44,0x28,0x10,0x10,0x10,0x10,0x00,
	0x7E,0x04,0x08,0x10,0x20,0x40,0x7E,0x00,0x18,0x10,0x10,0x10,0x10,0x10,0x18,0x00,
	0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x18,0x08,0x08,0x08,0x08,0x08,0x18,0x00,
	0x10,0x28,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,
	0x08,0x10,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x78,0x04,0x7C,0x84,0x84,0x7E,0x00,
	0x40,0x40,0x7C,0x42,0x42,0x42,0x3C,0x00,0x00,0x00,0x3C,0x42,0x40,0x42,0x3C,0x00,
	0x02,0x02,0x3E,0x42,0x42,0x42,0x3C,0x00,0x00,0x00,0x3C,0x42,0x7E,0x40,0x3E,0x00,
	0x0C,0x10,0x3E,0x10,0x10,0x10,0x10,0x00,0x00,0x3C,0x42,0x42,0x3E,0x02,0x7C,0x00,
	0x40,0x40,0x7C,0x42,0x42,0x42,0x42,0x00,0x18,0x18,0x00,0x08,0x08,0x08,0x08,0x00,
	0x06,0x06,0x00,0x02,0x42,0x42,0x3C,0x00,0x20,0x20,0x26,0x28,0x30,0x28,0x26,0x00,
	0x30,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x80,0xEC,0x92,0x92,0x92,0x92,0x00,
	0x00,0x40,0x78,0x44,0x44,0x44,0x44,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x3C,0x00,
	0x00,0x3C,0x42,0x42,0x7C,0x40,0x40,0x00,0x00,0x78,0x84,0x84,0x7C,0x04,0x06,0x00,
	0x00,0x00,0x5C,0x62,0x40,0x40,0x40,0x00,0x00,0x00,0x3E,0x40,0x3C,0x02,0x7C,0x00,
	0x00,0x10,0x7C,0x10,0x10,0x10,0x0E,0x00,0x00,0x00,0x42,0x42,0x42,0x42,0x3F,0x00,
	0x00,0x00,0x42,0x42,0x24,0x24,0x18,0x00,0x00,0x00,0x92,0x92,0x92,0x92,0x6C,0x00,
	0x00,0x00,0x42,0x24,0x18,0x24,0x42,0x00,0x00,0x00,0x42,0x42,0x3E,0x02,0x7C,0x00,
	0x00,0x00,0x7E,0x02,0x3C,0x40,0x7E,0x00,0x08,0x10,0x10,0x20,0x10,0x10,0x08,0x00,
	0x10,0x10,0x10,0x00,0x10,0x10,0x10,0x00,0x20,0x10,0x10,0x08,0x10,0x10,0x20,0x00,
	0x00,0x00,0x60,0x92,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x90,0x90,0x60,0x00,
	0x1E,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0xF0,0x00,
	0x00,0x00,0x00,0x80,0x40,0x60,0x60,0x00,0x00,0x00,0x00,0x30,0x78,0x30,0x00,0x00,
	0x20,0xF8,0x26,0x78,0xD0,0x80,0x7C,0x00,0x00,0x10,0x3A,0x1C,0x36,0x5A,0x36,0x00,
	0x00,0x00,0x44,0x42,0x42,0x42,0x30,0x00,0x00,0x3C,0x00,0x3C,0x42,0x02,0x3C,0x00,
	0x00,0x3C,0x00,0x7C,0x08,0x38,0x66,0x00,0x00,0x14,0x72,0x1C,0x32,0x52,0x34,0x00,
	0x00,0x28,0x2C,0x3A,0x62,0x16,0x10,0x00,0x00,0x08,0x5C,0x6A,0x4A,0x0C,0x18,0x00,
	0x00,0x08,0x0C,0x38,0x4C,0x4A,0x38,0x00,0x00,0x00,0x00,0x1C,0x62,0x02,0x1C,0x00,
	0x00,0x00,0x80,0x7E,0x00,0x00,0x00,0x00,0x28,0xF2,0x3C,0x6A,0xAA,0xB6,0xEC,0x00,
	0x80,0x88,0x84,0x84,0x82,0x92,0x70,0x00,0x78,0x00,0x3C,0xC2,0x02,0x04,0x78,0x00,
	0x78,0x00,0xFC,0x08,0x30,0x50,0x9E,0x00,0x2C,0xF2,0x20,0x7C,0xA2,0xA2,0xE4,0x00,
	0x28,0xF4,0x2A,0x4A,0x4A,0x88,0xB0,0x00,0x20,0xFC,0x12,0xFC,0x08,0xC2,0x7C,0x00,
	0x04,0x18,0x60,0x80,0xC0,0x30,0x0E,0x00,0x84,0xBE,0x84,0x84,0x84,0x84,0x58,0x00,
	0x00,0x7C,0x02,0x00,0x80,0x82,0x7E,0x00,0x20,0xFE,0x10,0x78,0x8C,0xC0,0x7C,0x00,
	0x80,0x80,0x80,0x80,0x82,0x84,0x78,0x00,0x04,0xFE,0x3C,0x44,0x7C,0x04,0x78,0x00,
	0x44,0x5E,0xF4,0x44,0x48,0x40,0x3E,0x00,0x44,0x58,0xE0,0x3E,0xC0,0x40,0x3C,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x90,0x90,0x60,0x00,
	0x1E,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0xF0,0x00,
	0x00,0x00,0x00,0x80,0x40,0x60,0x60,0x00,0x00,0x00,0x00,0x30,0x78,0x30,0x00,0x00,
	0x1E,0x62,0x1E,0x62,0x04,0x0C,0x30,0x00,0x00,0x00,0x1E,0x6A,0x0E,0x08,0x18,0x00,
	0x00,0x00,0x06,0x18,0x68,0x08,0x08,0x00,0x00,0x10,0x1C,0x72,0x42,0x04,0x38,0x00,
	0x00,0x00,0x0C,0x78,0x10,0x10,0x7E,0x00,0x00,0x08,0x08,0x7C,0x18,0x28,0x48,0x00,
	0x00,0x00,0x26,0x1A,0x72,0x10,0x08,0x00,0x00,0x00,0x0C,0x34,0x04,0x08,0x7E,0x00,
	0x00,0x00,0x78,0x04,0x3C,0x04,0x78,0x00,0x00,0x00,0x52,0x4A,0x22,0x04,0x38,0x00,
	0x00,0x00,0x80,0x7E,0x00,0x00,0x00,0x00,0x3C,0xC2,0x14,0x1C,0x10,0x30,0x60,0x00,
	0x02,0x0C,0x38,0xC8,0x08,0x08,0x08,0x00,0x60,0x3C,0xE2,0x82,0x82,0x04,0x38,0x00,
	0x00,0x1C,0x70,0x10,0x10,0x1C,0xE2,0x00,0x08,0xFE,0x18,0x38,0x68,0xC8,0x18,0x00,
	0x10,0x3E,0xD2,0x12,0x22,0x62,0xCC,0x00,0x20,0x3C,0xF0,0x1E,0xF0,0x08,0x08,0x00,
	0x10,0x3E,0x62,0xC2,0x04,0x0C,0x70,0x00,0x40,0x7E,0x44,0x84,0x84,0x08,0x30,0x00,
	0x3E,0xC2,0x02,0x02,0x02,0x02,0xFC,0x00,0x44,0x5E,0xE4,0x44,0x44,0x08,0xF0,0x00,
	0x60,0x12,0xC2,0x22,0x04,0x04,0xF8,0x00,0x3C,0xC6,0x0C,0x08,0x38,0x6C,0xC6,0x00,
	0x40,0x4E,0x72,0xC4,0x4C,0x40,0x3E,0x00,0x82,0x42,0x62,0x04,0x04,0x08,0x70,0x00,
	0x3C,0x42,0x72,0x8A,0x04,0x0C,0x70,0x00,0x0C,0xF8,0x10,0xFE,0x10,0x10,0x60,0x00,
	0x22,0xA2,0x92,0x42,0x04,0x08,0x70,0x00,0x3C,0x40,0x1E,0xE8,0x08,0x10,0x60,0x00,
	0x40,0x40,0x70,0x4C,0x42,0x40,0x40,0x00,0x08,0x3E,0xC8,0x08,0x08,0x18,0x70,0x00,
	0x00,0x1C,0x60,0x00,0x00,0x3C,0xC2,0x00,0x3C,0xC2,0x26,0x38,0x1C,0x36,0xE2,0x00,
	0x10,0x3C,0xC6,0x1C,0x38,0xD6,0x12,0x00,0x02,0x02,0x02,0x06,0x04,0x1C,0xF0,0x00,
	0x18,0x4C,0x44,0x46,0x42,0x82,0x82,0x00,0x80,0x86,0xBC,0xE0,0x80,0x80,0x7E,0x00,
	0x3C,0xC2,0x02,0x02,0x04,0x08,0x30,0x00,0x30,0x48,0x4C,0x84,0x86,0x02,0x02,0x00,
	0x10,0xFE,0x10,0x54,0x52,0x92,0x92,0x00,0x3C,0xC2,0x02,0x44,0x28,0x10,0x0C,0x00,
	0x70,0x0C,0x60,0x18,0xC4,0x30,0x0E,0x00,0x30,0x40,0x4C,0x84,0x8E,0xBA,0x62,0x00,
	0x04,0x04,0x64,0x18,0x0C,0x16,0xE2,0x00,0x1C,0xE0,0x3E,0xE0,0x20,0x20,0x1E,0x00,
	0x4E,0x52,0x62,0xE4,0x20,0x10,0x18,0x00,0x1C,0x64,0x04,0x08,0x08,0x10,0xFE,0x00,
	0x1C,0x62,0x02,0x3E,0x02,0x02,0x7C,0x00,0x3C,0xC0,0x3E,0xC2,0x02,0x04,0x78,0x00,
	0x44,0x42,0x42,0x42,0x22,0x04,0x78,0x00,0x50,0x50,0x52,0x52,0x52,0x54,0x88,0x00,
	0x80,0x80,0x82,0x82,0x84,0x88,0xF0,0x00,0x1C,0xE2,0x82,0x82,0x82,0x4C,0x74,0x00,
	0x3C,0xC2,0x82,0x82,0x02,0x04,0x38,0x00,0xC0,0x62,0x22,0x02,0x04,0x08,0xF0,0x00,
	0x00,0x00,0x00,0x00,0x0A,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x02,0x05,0x02,0x00,
	0x20,0xF8,0x40,0x5E,0x80,0xA0,0x9E,0x00,0x20,0xFE,0x40,0x7C,0xC2,0x86,0x3C,0x00,
	0x00,0x3C,0xC6,0x02,0x02,0x0C,0x38,0x00,0x0E,0xF8,0x10,0x20,0x20,0x10,0x0E,0x00,
	0x40,0x4C,0x30,0x40,0x80,0x80,0x7E,0x00,0x44,0xF2,0x4A,0x9C,0xA4,0xA6,0x3A,0x00,
	0x40,0x5C,0x82,0x80,0xA0,0xA0,0x9E,0x00,0x48,0x7C,0x52,0xB2,0xBE,0xAA,0x4C,0x00,
	0x20,0xFC,0x32,0x62,0xEE,0xAA,0x2C,0x00,0x38,0x54,0x92,0x92,0xB2,0xA2,0x4C,0x00,
	0x44,0x5E,0x84,0x9C,0xA4,0xA6,0x9C,0x00,0x28,0xEE,0x44,0x84,0x84,0x44,0x38,0x00,
	0x78,0x10,0x64,0x34,0x8A,0x8A,0x30,0x00,0x30,0x58,0x48,0x84,0x84,0x02,0x02,0x00,
	0xBC,0x88,0xBE,0x84,0xBC,0xA6,0x9C,0x00,0x68,0x1E,0x68,0x1E,0x78,0x8C,0x7A,0x00,
	0x70,0x14,0x7C,0x96,0x94,0x94,0x68,0x00,0x2C,0xF2,0x60,0xA0,0xA2,0xC2,0x7C,0x00,
	0x48,0x7C,0x6A,0xAA,0xB2,0xB2,0x6C,0x00,0x10,0xF8,0x20,0xF8,0x22,0x22,0x1C,0x00,
	0x48,0x5C,0x6A,0xC2,0x64,0x20,0x18,0x00,0x10,0xBC,0xD6,0xCA,0xAA,0x1C,0x70,0x00,
	0x10,0x1C,0x12,0x70,0x9C,0x92,0x70,0x00,0xE0,0x18,0x40,0x7C,0xC2,0x82,0x3C,0x00,
	0x44,0x42,0x82,0xA2,0x62,0x04,0x78,0x00,0x7C,0x38,0x7C,0xC2,0xBA,0x26,0x3C,0x00,
	0x48,0xD4,0x64,0x64,0xC4,0xC4,0x46,0x00,0x7C,0x30,0x7C,0xC2,0x82,0x06,0x3C,0x00,
	0x20,0xFC,0x32,0x62,0xE2,0xA2,0x2C,0x00,0x10,0x30,0x60,0x72,0xD2,0x92,0x9C,0x00,
	0x00,0x00,0x00,0x00,0x0A,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x02,0x05,0x02,0x00
};

bool bios_install(void)
{
	//=== Install the reverse engineered bios
	int i;

	uint32 vectable[] =
	{
		0xFF27A2,		//0			VECT_SHUTDOWN
		0xFF1030,		//1			VECT_CLOCKGEARSET
		0xFF1440,		//2			VECT_RTCGET
		0xFF12B4,		//3			?
		0xFF1222,		//4			VECT_INTLVSET
		0xFF8D8A,		//5			VECT_SYSFONTSET
		0xFF6FD8,		//6			VECT_FLASHWRITE
		0xFF7042,		//7			VECT_FLASHALLERS
		0xFF7082,		//8			VECT_FLASHERS
		0xFF149B,		//9			VECT_ALARMSET
		0xFF1033,		//10		?			
		0xFF1487,		//11		VECT_ALARMDOWNSET
		0xFF731F,		//12		?
		0xFF70CA,		//13		VECT_FLASHPROTECT
		0xFF17C4,		//14		VECT_GEMODESET	
		0xFF1032,		//15		?

		0xFF2BBD,		//0x10		VECT_COMINIT	
		0xFF2C0C,		//0x11		VECT_COMSENDSTART
		0xFF2C44,		//0x12		VECT_COMRECIVESTART
		0xFF2C86,		//0x13		VECT_COMCREATEDATA
		0xFF2CB4,		//0x14		VECT_COMGETDATA
		0xFF2D27,		//0x15		VECT_COMONRTS
		0xFF2D33,		//0x16		VECT_COMOFFRTS
		0xFF2D3A,		//0x17		VECT_COMSENDSTATUS	
		0xFF2D4E,		//0x18		VECT_COMRECIVESTATUS
		0xFF2D6C,		//0x19		VECT_COMCREATEBUFDATA
		0xFF2D85,		//0x1a		VECT_COMGETBUFDATA
	};

	//System Call Table, install iBIOSHLE instructions
	for (i = 0; i <= 0x1A; i++)
	{
		MDFN_en32lsb(&ngpc_bios[0xFE00 + (i * 4)], vectable[i]);
		ngpc_bios[vectable[i] & 0xFFFF] = 0x1F;	//iBIOSHLE
	}

	//System Font
	memcpy(ngpc_bios + 0x8DCF, font, 0x800);

	//Default Interrupt handler
	ngpc_bios[0x23DF] = 0x07;  //RETI

	// ==========
		
	//Install a Quick and Dirty Bios
	ngpc_bios[0xFFFE] = 0x68; // - JR 0xFFFFFE (Infinite loop!)
	ngpc_bios[0xFFFF] = 0xFE;

	return TRUE;	//Success
}

//=============================================================================
