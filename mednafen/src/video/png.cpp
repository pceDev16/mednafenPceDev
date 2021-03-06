/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "video-common.h"

#include <zlib.h>
#include "png.h"

namespace Mednafen
{

void PNGWrite::WriteChunk(FileStream &pngfile, uint32 size, const char *type, const uint8 *data)
{
 uint32 crc;
 uint8 tempo[4];

 MDFN_en32msb(tempo, size);

 pngfile.write(tempo, 4);
 pngfile.write(type, 4);

 if(size)
  pngfile.write(data, size);

 crc = crc32(0, (uint8 *)type, 4);
 if(size)
  crc = crc32(crc,data,size);

 MDFN_en32msb(tempo, crc);

 pngfile.write(tempo, 4);
}

PNGWrite::~PNGWrite()
{

}

PNGWrite::PNGWrite(const std::string& path, const MDFN_Surface *src, const MDFN_Rect &rect, const int32 *LineWidths) : ownfile(path, FileStream::MODE_WRITE_SAFE)
{
 WriteIt(ownfile, src, rect, LineWidths);
 ownfile.close();
}

INLINE void PNGWrite::EncodeImage(const MDFN_Surface *src, const MDFN_PixelFormat &format, const MDFN_Rect &rect, const int32 *LineWidths, const int png_width)
{
 const int32 pitchinpix = src->pitchinpix;
 uint8 *tmp_inc;

 tmp_buffer.resize((png_width * ((format.opp == 1) ? 1 : 3) + 1) * rect.h);

 tmp_inc = &tmp_buffer[0];

 for(int y = 0; y < rect.h; y++)
 {
  *tmp_inc = 0;
  tmp_inc++;
  int line_width = rect.w;
  int x_base = rect.x;

  if(LineWidths && LineWidths[0] != ~0)
  {
   line_width = LineWidths[y + rect.y];
  }

  for(int x = 0; MDFN_LIKELY(x < line_width); x++)
  {
   int r, g, b;

   if(format.opp == 1)
   {
    tmp_inc[0] = src->pixels8[(y + rect.y) * pitchinpix + (x + x_base)];
    tmp_inc++;
   }
   else
   {
    if(format.opp == 2)
     format.DecodeColor(src->pixels16[(y + rect.y) * pitchinpix + (x + x_base)], r, g, b);
    else
     format.DecodeColor(src->pixels[(y + rect.y) * pitchinpix + (x + x_base)], r, g, b);

    tmp_inc[0] = r;
    tmp_inc[1] = g;
    tmp_inc[2] = b;
    tmp_inc += 3;
   }
  }

  for(int x = line_width; x < png_width; x++)
  {
   if(format.opp == 1)
   {
    tmp_inc[0] = 0;
    tmp_inc++;
   }
   else
   {
    tmp_inc[0] = tmp_inc[1] = tmp_inc[2] = 0;
    tmp_inc += 3;
   }
  }
 }
}

void PNGWrite::WriteIt(FileStream &pngfile, const MDFN_Surface *src, const MDFN_Rect &rect_in, const int32 *LineWidths)
{
 uLongf compmemsize;
 int png_width;
 const MDFN_PixelFormat format = src->format;
 const MDFN_Rect rect = rect_in;

 if(LineWidths && LineWidths[0] != ~0)
 {
  png_width = 0;

  for(int y = 0; y < rect.h; y++)
  {
   const int32 cur_width = LineWidths[rect.y + y];

   if(cur_width > png_width)
    png_width = cur_width;
  }
 }  
 else
  png_width = rect.w;

 if(!rect.h)
  throw(MDFN_Error(0, "Refusing to save a zero-height PNG."));

 if(!png_width)
  throw(MDFN_Error(0, "Refusing to save a zero-width PNG."));

 compmemsize = (uLongf)( (rect.h * (png_width + 1) * 3 * 1.001 + 1) + 12 );

 compmem.resize(compmemsize);

 {
  static const uint8 header[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
  pngfile.write(header, 8);
 }

 {
  uint8 chunko[13];

  MDFN_en32msb(&chunko[0], png_width);	// Width

  MDFN_en32msb(&chunko[4], rect.h);	// Height

  chunko[8]=8;				// Bit depth

  if(format.opp == 1)
   chunko[9]=3;				// Color type; palette index
  else 
   chunko[9]=2;				// Color type; RGB triplet

  chunko[10]=0;				// compression: deflate
  chunko[11]=0;				// Basic adaptive filter set(though none are used).
  chunko[12]=0;				// No interlace.

  WriteChunk(pngfile, 13, "IHDR", chunko);
 }

 if(format.opp == 1)
 {
  uint8 chunko[256 * 3];

  for(int i = 0; i < 256; i++)
  {
   uint8 r, g, b;

   src->format.DecodePColor(src->palette[i], r, g, b);

   chunko[(i * 3) + 0] = r;
   chunko[(i * 3) + 1] = g;
   chunko[(i * 3) + 2] = b;
  }

  WriteChunk(pngfile, 256 * 3, "PLTE", chunko);
 }

 // pHYs chunk
 #if 0
 {
  uint8 chunko[9];
  uint32 ppx, ppy;

  //ppx = png_width / MDFNGameInfo->nominal_width;
  //ppy = 1;	//rect->h / rect->h

  ppx = png_width;
  ppy = MDFNGameInfo->nominal_width;

  MDFN_en32msb(&chunko[0], ppx);
  MDFN_en32msb(&chunko[4], ppy);

  //printf("%08x %08x, %04x %04x\n", ppx, ppy, *(uint32 *)&chunko[0], *(uint32 *)&chunko[4]);

  chunko[8] = 0;

  WriteChunk(pngfile, 9, "pHYs", chunko);
 }
 #endif


 // IDAT chunk
 {
  //uint32 st = MDFND_GetTime();

  if(MDFN_LIKELY(format.colorspace == MDFN_COLORSPACE_RGB && format.opp == 4))
   EncodeImage(src, format, rect, LineWidths, png_width);
  else
   EncodeImage(src, format, rect, LineWidths, png_width);

  //printf("%u\n", MDFND_GetTime() - st);

  if(compress(&compmem[0], &compmemsize, &tmp_buffer[0], rect.h * (png_width * ((format.opp == 1) ? 1 : 3) + 1)) != Z_OK)
  {
   throw(MDFN_Error(0, "zlib error"));	// TODO: verbosify
  }

  WriteChunk(pngfile, compmemsize, "IDAT", &compmem[0]);
 }
 //
 //
 //

 WriteChunk(pngfile, 0, "IEND", 0);
}

}
