from fontLib import *

if __name__ == '__main__':
    # Be sure to place 'helvetica.ttf' (or any other ttf / otf font file) in the working directory.
    fnt_size = 110
    max_height = 103
    max_width = 104
    space_width = 33
    max_descent = 22
    font_prefix = 'font110'
    fontRender(fnt_size, max_height, max_width, max_descent, space_width, font_prefix, 'included')
