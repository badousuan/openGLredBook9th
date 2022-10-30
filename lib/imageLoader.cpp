//
//  main.c
//  AIFilePaser
//
//  Created by Lei Yue on 2022/9/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string>


#include <stdio.h>
#include "vgl.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
 


unsigned char * loadImg(const char * filename, GLenum &format, int &width, int &height) {

    int channels_in_file = 0;
    int desired_channels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char * img = stbi_load(filename,&width, &height, &channels_in_file, desired_channels);
    if(channels_in_file == 3) {
        format = GL_RGB;
    } else if(channels_in_file == 4) {
        format = GL_RGBA;
    }
    return img;
}
