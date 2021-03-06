/******************************************************************************/
/* Mednafen - Multi-system Emulator                                           */
/******************************************************************************/
/* nnx.h:
**  Copyright (C) 2005-2016 Mednafen Team
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* Nearest-neighbor simple scalers */
// FIXME: non-32bpp scaling not working yet.

#include "main.h"
#include "nnx.h"

template<typename T>
static void t_nnx(int factor, const MDFN_Surface *src, const MDFN_Rect *src_rect, MDFN_Surface *dest, const MDFN_Rect *dest_rect)
{
 const T *source_pixies = src->pix<T>() + src_rect->y * src->pitchinpix;
 source_pixies += src_rect->x;
 int source_pitch = src->pitchinpix;
 int source_pitch_diff = source_pitch - src_rect->w;

 T *dest_pixies = dest->pix<T>() + dest_rect->y * dest->pitchinpix;
 dest_pixies += dest_rect->x;
 int dest_pitch = dest->pitchinpix;
 int dest_pitch_diff = dest_pitch - dest_rect->w + dest_pitch * (factor - 1);

 int max_x = src_rect->w;

 switch(factor)
 {
  case 2:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    dest_pixies[0] = dest_pixies[1] = dest_pixies[dest_pitch] = dest_pixies[dest_pitch + 1] = *source_pixies;
    source_pixies++;
    dest_pixies += 2;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

  case 3:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    dest_pixies[0] = dest_pixies[1] = dest_pixies[2] = 
    dest_pixies[dest_pitch] = dest_pixies[dest_pitch + 1] = dest_pixies[dest_pitch + 2] = 
    dest_pixies[dest_pitch << 1] = dest_pixies[(dest_pitch << 1) + 1] = dest_pixies[(dest_pitch << 1) + 2] = *source_pixies;
    source_pixies++;
    dest_pixies += 3;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

  case 4:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    dest_pixies[0] = dest_pixies[1] = dest_pixies[2] = dest_pixies[3] =
    dest_pixies[dest_pitch] = dest_pixies[dest_pitch + 1] = dest_pixies[dest_pitch + 2] = dest_pixies[dest_pitch + 3] =
    dest_pixies[dest_pitch << 1] = dest_pixies[(dest_pitch << 1) + 1] = dest_pixies[(dest_pitch << 1) + 2] = dest_pixies[(dest_pitch << 1) + 3] = 
    dest_pixies[(dest_pitch << 1) + dest_pitch] = dest_pixies[(dest_pitch << 1) + dest_pitch + 1] = dest_pixies[(dest_pitch << 1) + dest_pitch + 2] = dest_pixies[(dest_pitch << 1) + dest_pitch + 3] = *source_pixies;
    source_pixies++;
    dest_pixies += 4;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

  case 5:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    // Line 0
    dest_pixies[0] = dest_pixies[1] = dest_pixies[2] = dest_pixies[3] = dest_pixies[4] =

    // Line 1
    dest_pixies[dest_pitch] = dest_pixies[dest_pitch + 1] = dest_pixies[dest_pitch + 2] = dest_pixies[dest_pitch + 3] = dest_pixies[dest_pitch + 4] =

    // Line 2
    dest_pixies[dest_pitch << 1] = dest_pixies[(dest_pitch << 1) + 1] = dest_pixies[(dest_pitch << 1) + 2] = dest_pixies[(dest_pitch << 1) + 3] = dest_pixies[(dest_pitch << 1) + 4] =  

    // Line 3
    dest_pixies[(dest_pitch << 1) + dest_pitch] = dest_pixies[(dest_pitch << 1) + dest_pitch + 1] = dest_pixies[(dest_pitch << 1) + dest_pitch + 2] = dest_pixies[(dest_pitch << 1) + dest_pitch + 3] = dest_pixies[(dest_pitch << 1) + dest_pitch + 4] = *source_pixies;

    // Line 4
    dest_pixies[(dest_pitch << 2)] = dest_pixies[(dest_pitch << 2) + 1] = dest_pixies[(dest_pitch << 2) + 2] = dest_pixies[(dest_pitch << 2) + 3] = dest_pixies[(dest_pitch << 2) + 4] = *source_pixies;

    source_pixies++;
    dest_pixies += 5;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

 }
}

template<typename T>
static void t_nnyx(int factor, const MDFN_Surface *src, const MDFN_Rect *src_rect, MDFN_Surface *dest, const MDFN_Rect *dest_rect)
{
 const T *source_pixies = src->pix<T>() + src_rect->y * src->pitchinpix;
 source_pixies += src_rect->x;
 int source_pitch = src->pitchinpix;
 int source_pitch_diff = source_pitch - src_rect->w;

 T *dest_pixies = dest->pix<T>() + dest_rect->y * dest->pitchinpix;
 dest_pixies += dest_rect->x;
 int dest_pitch = dest->pitchinpix;
 int dest_pitch_diff = dest_pitch - dest_rect->w + dest_pitch * (factor - 1);

 int max_x = src_rect->w;

 switch(factor)
 {
  case 2:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    dest_pixies[0] = dest_pixies[dest_pitch] = *source_pixies;
    source_pixies++;
    dest_pixies ++;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

  case 3:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    dest_pixies[0] =
    dest_pixies[dest_pitch] = 
    dest_pixies[dest_pitch << 1] = *source_pixies;
    source_pixies++;
    dest_pixies ++;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

  case 4:
  for(int y = src_rect->h; y; y--)
  {
   for(int x = max_x; x; x--)
   {
    dest_pixies[0] = 
    dest_pixies[dest_pitch] = 
    dest_pixies[dest_pitch << 1] = 
    dest_pixies[(dest_pitch << 1) + dest_pitch] = *source_pixies;
    source_pixies++;
    dest_pixies ++;
   }
   dest_pixies += dest_pitch_diff;
   source_pixies += source_pitch_diff;
  }
  break;

 }
}

void nnx(int factor, const MDFN_Surface* src, const MDFN_Rect& src_rect, MDFN_Surface* dest, const MDFN_Rect& dest_rect)
{
 switch(src->format.opp)
 {
/*
  case 1:
	t_nnx<uint8>(factor, src, src_rect, dest, dest_rect);
	break;
*/
  case 2:
	t_nnx<uint16>(factor, src, &src_rect, dest, &dest_rect);
	break;

  case 4:
	t_nnx<uint32>(factor, src, &src_rect, dest, &dest_rect);
	break;
 }
}

void nnyx(int factor, const MDFN_Surface* src, const MDFN_Rect& src_rect, MDFN_Surface* dest, const MDFN_Rect& dest_rect)
{
 switch(src->format.opp)
 {
/*
  case 1:
        t_nnyx<uint8>(factor, src, src_rect, dest, dest_rect);
        break;
*/
  case 2:
        t_nnyx<uint16>(factor, src, &src_rect, dest, &dest_rect);
        break;

  case 4:
        t_nnyx<uint32>(factor, src, &src_rect, dest, &dest_rect);
        break;
 }
}

