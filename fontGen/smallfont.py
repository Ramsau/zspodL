from fontLib import *

if __name__ == '__main__':
    # Be sure to place 'helvetica.ttf' (or any other ttf / otf font file) in the working directory.
    fnt_size = 40
    max_height = 38
    max_width = 38
    max_descent = 8
    fnt = Font('C059-BdIta.ttf', fnt_size)

    print('#include "bigfont.h"\n#include <avr/pgmspace.h>\n\n')

    arr = 'const BigFont font40 = {\n%d,\n%d,\n{' % (max_height, max_width)
    count = 0
    for i in list(range(97, 123)) + [ord(' '), ord('ä'), ord('ö'), ord('ü')]:
        # repr(fnt.render_text(char))
        glyph = fnt.render_text(chr(i), height=max_height, baseline=max_descent)
        repr, array_entry = glyph.arrayize(font_prefix='font40')
        print(repr)
        arr += array_entry

        max_width = max(max_width, glyph.width)
        count += 1

    arr += '},\n};'
    print(arr)
