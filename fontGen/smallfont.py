from fontLib import *

if __name__ == '__main__':
    # Be sure to place 'helvetica.ttf' (or any other ttf / otf font file) in the working directory.
    fnt_size = 40
    max_height = 38
    max_width = 38
    space_width = 12
    max_descent = 8
    font_prefix = 'font40'
    fontRender(fnt_size, max_height, max_width, max_descent, space_width, font_prefix, 'back')
