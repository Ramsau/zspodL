#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Needs freetype-py>=1.0

# For more info see:
# http://dbader.org/blog/monochrome-font-rendering-with-freetype-and-python

# The MIT License (MIT)
#
# Copyright (c) 2013 Daniel Bader (http://dbader.org)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import freetype


class Bitmap(object):
    """
    A 2D bitmap image represented as a list of byte values. Each byte indicates the state
    of a single pixel in the bitmap. A value of 0 indicates that the pixel is `off`
    and any other value indicates that it is `on`.
    """

    def __init__(self, width, height, bearing=None, advance_width=None, pixels=None, char=' '):
        self.width = width
        self.advance_width = advance_width if advance_width else self.width
        self.bearing = bearing if bearing else 0
        self.height = height
        self.pixels = pixels or bytearray(width * height)
        self.char = char

    def arrayize(self, font_prefix=''):
        """Return a string representation of the bitmap's pixels."""

        char_repr = self.char
        if ord(char_repr) in range(1, 32):
            char_repr = ' '

        rows = "const uint8_t %s_char_%d[] PROGMEM = { // '%c'\n" % (font_prefix, ord(self.char), char_repr)
        rows += '    {0:#04x}, // width\n'.format(self.width)
        rows += '    {0:#04x}, // bearing\n'.format(self.bearing if self.bearing > 0 else 0xff + self.bearing)
        rows += '    {0:#04x}, // advance width\n'.format(self.advance_width)
        blankLines = -1

        for y in range(self.height):
            rows += ''
            current_color = 0

            vals = '    '
            for x in range(self.width):
                new_color = self.pixels[y * self.width + x]
                if new_color != current_color:
                    # save positions where color is switched
                    vals += '{0:#04x}, '.format(x)
                    current_color = new_color

                    # break off the blank line counter
                    if blankLines != -1:
                        rows += '    0xff, {0:#04x}, // blank lines\n'.format(blankLines)
                        blankLines = -1

            # close open color at end of line
            if current_color:
                vals += '{0:#04x}, '.format(self.width)

            # leave out blank lines if applicable
            if not vals.strip():
                if blankLines == -1:
                    blankLines = 1
                else:
                    blankLines += 1
            else:
                if blankLines != -1:
                    rows += '    0xff, {0:#04x}, '.format(blankLines)
                    blankLines = -1
            # add vals and unified spacing
            rows += vals
            rows += ' ' * (60 - len(vals))

            rows += '// '
            for x in range(self.width):
                rows += '#' if self.pixels[y * self.width + x] else ' '
            rows += '\n'

        # add finishing empty lines
        rows += '0xff, 0xff,// blank lines\n '

        rows += '};\n'

        width_next8 = ((self.width + 7) & -8)

        array_entry = \
        '    (const uint8_t*)%s_char_%d,\n' % (font_prefix, ord(self.char))

        return rows, array_entry

    def bitblt(self, src, x, y):
        """Copy all pixels from `src` into this bitmap"""
        srcpixel = 0
        dstpixel = y * self.width + x
        row_offset = self.width - src.width

        for sy in range(src.height):
            for sx in range(src.width):
                # Perform an OR operation on the destination pixel and the source pixel
                # because glyph bitmaps may overlap if character kerning is applied, e.g.
                # in the string "AVA", the "A" and "V" glyphs must be rendered with
                # overlapping bounding boxes.
                try:
                    self.pixels[dstpixel] = src.pixels[srcpixel]  # self.pixels[dstpixel]
                except:
                    pass
                srcpixel += 1
                dstpixel += 1
            dstpixel += row_offset


class Glyph(object):
    def __init__(self, pixels, width, height, top, bearing, advance_width):
        # The advance width determines where to place the next character horizontally,
        # that is, how many pixels we move to the right to draw the next glyph.
        self.advance_width = advance_width

        self.bitmap = Bitmap(width, height, bearing, advance_width, pixels)

        # The glyph bitmap's top-side bearing, i.e. the vertical distance from the
        # baseline to the bitmap's top-most scanline.
        self.top = top

        # Ascent and descent determine how many pixels the glyph extends
        # above or below the baseline.
        self.descent = max(0, self.height - self.top)
        self.ascent = max(0, max(self.top, self.height) - self.descent)


    @property
    def width(self):
        return self.bitmap.width

    @property
    def height(self):
        return self.bitmap.height

    @staticmethod
    def from_glyphslot(slot):
        """Construct and return a Glyph object from a FreeType GlyphSlot."""
        pixels = Glyph.unpack_mono_bitmap(slot.bitmap)
        width, height = slot.bitmap.width, slot.bitmap.rows
        top = slot.bitmap_top

        # The advance width is given in FreeType's 26.6 fixed point format,
        # which means that the pixel values are multiples of 64.
        advance_width = slot.advance.x // 64
        bearing = slot.metrics.horiBearingX // 64

        return Glyph(pixels, width, height, top, bearing, advance_width)

    @staticmethod
    def unpack_mono_bitmap(bitmap):
        """
        Unpack a freetype FT_LOAD_TARGET_MONO glyph bitmap into a bytearray where each
        pixel is represented by a single byte.
        """
        # Allocate a bytearray of sufficient size to hold the glyph bitmap.
        data = bytearray(bitmap.rows * bitmap.width)

        # Iterate over every byte in the glyph bitmap. Note that we're not
        # iterating over every pixel in the resulting unpacked bitmap --
        # we're iterating over the packed bytes in the input bitmap.
        for y in range(bitmap.rows):
            for byte_index in range(bitmap.pitch):

                # Read the byte that contains the packed pixel data.
                byte_value = bitmap.buffer[y * bitmap.pitch + byte_index]

                # We've processed this many bits (=pixels) so far. This determines
                # where we'll read the next batch of pixels from.
                num_bits_done = byte_index * 8

                # Pre-compute where to write the pixels that we're going
                # to unpack from the current byte in the glyph bitmap.
                rowstart = y * bitmap.width + byte_index * 8

                # Iterate over every bit (=pixel) that's still a part of the
                # output bitmap. Sometimes we're only unpacking a fraction of a byte
                # because glyphs may not always fit on a byte boundary. So we make sure
                # to stop if we unpack past the current row of pixels.
                for bit_index in range(min(8, bitmap.width - num_bits_done)):
                    # Unpack the next pixel from the current glyph byte.
                    bit = byte_value & (1 << (7 - bit_index))

                    # Write the pixel to the output bytearray. We ensure that `off`
                    # pixels have a value of 0 and `on` pixels have a value of 1.
                    data[rowstart + bit_index] = 1 if bit else 0

        return data




class Font(object):

    def __init__(self, filename, size):
        self.face = freetype.Face(filename)
        self.face.set_pixel_sizes(0, size)

    def glyph_for_character(self, char):
        # Let FreeType load the glyph for the given character and tell it to render
        # a monochromatic bitmap representation.
        self.face.load_char(char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO)
        return Glyph.from_glyphslot(self.face.glyph)

    def render_character(self, char):
        glyph = self.glyph_for_character(char)
        return glyph.bitmap

    def kerning_offset(self, previous_char, char):
        """
        Return the horizontal kerning offset in pixels when rendering `char`
        after `previous_char`.

        Use the resulting offset to adjust the glyph's drawing position to
        reduces extra diagonal whitespace, for example in the string "AV" the
        bitmaps for "A" and "V" may overlap slightly with some fonts. In this
        case the glyph for "V" has a negative horizontal kerning offset as it is
        moved slightly towards the "A".
        """
        kerning = self.face.get_kerning(previous_char, char)

        # The kerning offset is given in FreeType's 26.6 fixed point format,
        # which means that the pixel values are multiples of 64.
        return kerning.x // 64

    def text_dimensions(self, text):
        """Return (width, height, baseline) of `text` rendered in the current font."""
        width = 0
        max_ascent = 0
        max_descent = 0
        bearing = 0
        advance_width = 0
        previous_char = None

        # For each character in the text string we get the glyph
        # and update the overall dimensions of the resulting bitmap.
        for char in text:
            glyph = self.glyph_for_character(char)
            max_ascent = max(max_ascent, glyph.ascent)
            max_descent = max(max_descent, glyph.descent)
            kerning_x = self.kerning_offset(previous_char, char)

            # With kerning, the advance width may be less than the width of the glyph's bitmap.
            # Make sure we compute the total width so that all of the glyph's pixels
            # fit into the returned dimensions.
            width += max(glyph.advance_width + kerning_x, glyph.width + kerning_x)

            advance_width += glyph.advance_width
            bearing = glyph.bitmap.bearing

            previous_char = char

        height = max_ascent + max_descent

        return (width, height, max_descent, bearing, advance_width)

    def render_text(self, text, width=None, height=None, baseline=None):
        """
        Render the given `text` into a Bitmap and return it.

        If `width`, `height`, and `baseline` are not specified they are computed using
        the `text_dimensions' method.
        """
        if None in (width, height, baseline):
            width, _, _, bearing, advance_width = self.text_dimensions(text)

        x = 0
        previous_char = None
        outbuffer = Bitmap(width, height, bearing=bearing, advance_width=advance_width, char=text)

        for char in text:
            glyph = self.glyph_for_character(char)

            # Take kerning information into account before we render the
            # glyph to the output bitmap.
            x += self.kerning_offset(previous_char, char)

            # The vertical drawing position should place the glyph
            # on the baseline as intended.
            y = height - glyph.ascent - baseline

            outbuffer.bitblt(glyph.bitmap, x, y)

            x += glyph.advance_width
            previous_char = char

        return outbuffer


def fontRender(fnt_size, max_height, max_width, max_descent, space_width, font_prefix, useSpace):
    fnt = Font('C059-BdIta.ttf', fnt_size)

    number_location = 0 if (useSpace == 'included') else ord('z') - ord('A') + 1

    print('#include "bigfont.h"\n#include <avr/pgmspace.h>\n\n')

    arr = 'const BigFont %s = {\n%d,\n%d,\n%d, \n%d, \n{' % (font_prefix, max_height, max_width, space_width, number_location)
    count = 0

    letters = []
    if useSpace == 'included':
        letters += range(ord('0'), ord('9') + 1)
        num_free = ord('Z') - ord('A') - len(letters) + 1
        letters += [0] * num_free
    else:
        letters += range(ord('A'), ord('Z') + 1)

    letters += [ord(','), ord('.'), ord(':'), ord('ä'), ord('ö'), ord('ü')]
    letters += range(ord('a'), ord('z') + 1)

    if useSpace == 'back':
        letters += range(ord('0'), ord('9') + 1)

    for i in letters:
        if i == 0:
            arr += "    (const uint8_t*)0,\n"
            continue
        # repr(fnt.render_text(char))
        glyph = fnt.render_text(chr(i), height=max_height, baseline=max_descent)
        repr, array_entry = glyph.arrayize(font_prefix=font_prefix)
        print(repr)
        arr += array_entry

        max_width = max(max_width, glyph.width)
        count += 1

    arr += '},\n};'
    print(arr)
