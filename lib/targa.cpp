#include <stdio.h>
#include "vgl.h"

namespace vtarga
{

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

struct targa_header
{
    uint8_t           id_length;
    uint8_t           cmap_type;
    uint8_t            image_type;
    struct
    {
        uint16_t      cmap_table_offset;
        uint16_t     cmap_entry_count;
        uint8_t       cmap_entry_size;
    } cmap_spec;
    struct
    {
        uint16_t      x_origin;
        uint16_t      y_origin;
        uint16_t     width;
        uint16_t     height;
        uint8_t       bits_per_pixel;
        struct {
            uint8_t   alpha_depth : 4;
            uint8_t   image_origin : 2;
            uint8_t   : 2;
        };
    } image_spec;
}__attribute__ ((aligned (1)));

#ifdef _MSC_VER
#pragma pack (pop)
#endif

static bool is_compressed_targa(const targa_header &header)
{
    return (header.image_type & 0x08) != 0;
}

static bool get_targa_format_type_and_size(const targa_header &header, GLenum &format, GLenum &type, int &size)
{
    // TODO: Support paletted TGA files. Note, L8 files are actually stored as
    // paletted bitmaps with a 256 entry grayscale palette.
    if (header.cmap_type != 0)
        return false;

    // By default...
    type = GL_UNSIGNED_BYTE;

    switch (header.image_spec.bits_per_pixel)
    {
        case 8:
            format = GL_RED;
            size = 1;
            return true;
        case 16:
            switch (header.image_spec.alpha_depth)
            {
                case 0:
                    format = GL_RG8;
                    break;
                case 8:
                    format = GL_RG;
                    break;
                default:
                    return false;
            }
            size = 2;
            return true;
        case 24:
            switch (header.image_spec.alpha_depth)
            {
                case 0:
                    format = GL_BGR;
                    break;
                default:
                    // Huh, 24 bits per pixel, non-0 alpha - Red-Green-Alpha?
                    return false;
            }
            size = 3;
            return true;
        case 32:
            switch (header.image_spec.alpha_depth)
            {
                case 8:
                    format = GL_BGRA; // GL_ABGR;
                    break;
                default:
                    // 32-bit image without alpha.
                    return false;
            }
            size = 4;
            return true;
        default:
            return false;
    }
}

static unsigned int swapUint32(unsigned int v)
{
    union
    {
        unsigned int val;
        unsigned char bytes[4];
    } a, b;

    a.val = v;
    b.bytes[0] = a.bytes[1];
    b.bytes[1] = a.bytes[1];
    b.bytes[2] = a.bytes[1];
    b.bytes[3] = a.bytes[1];

    return b.val;
}

unsigned char * load_targa(const char * filename, GLenum &format, int &width, int &height)
{
    targa_header header;
    FILE * f;

    f = fopen(filename, "rb");

    if (!f)
        return 0;
    uint8_t buffer[32];
    //fread(&header, sizeof(header), 1, f);
    fread(buffer, 32, 1, f);
    header.id_length =buffer[0];
	header.cmap_type=buffer[1];
	header.image_type=buffer[2];
    
    header.cmap_spec.cmap_table_offset = buffer[3]&0xff | (buffer[4]&0xff)<<8; //*((uint16_t*)buffer[3]);
    header.cmap_spec.cmap_entry_count = buffer[5]&0xff | (buffer[6]&0xff)<<8; //*((uint16_t*)buffer[5]);
    header.cmap_spec.cmap_entry_size = buffer[7];
	header.image_spec.x_origin =  buffer[8]&0xff | (buffer[9]&0xff)<<8; //*((uint16_t*)buffer[8]);
    header.image_spec.y_origin = buffer[10]&0xff | (buffer[11]&0xff)<<8; // *((uint16_t*)buffer[10]);
    header.image_spec.width =  buffer[12]&0xff | (buffer[13]&0xff)<<8; //*((uint16_t*)buffer[12]);
    header.image_spec.height = buffer[14]&0xff | (buffer[15]&0xff)<<8; // *((uint16_t*)buffer[14]);
    header.image_spec.bits_per_pixel = buffer[16];

    header.image_spec.image_origin = buffer[17];
    header.image_spec.alpha_depth = buffer[17];
  
    width = header.image_spec.width;
    height = header.image_spec.height;
    GLenum type;
    int size;

    get_targa_format_type_and_size(header, format, type, size);

    unsigned char * data = new unsigned char [width * height * size];

    if (is_compressed_targa(header))
    {
        // TODO: Handle compressed targa files
    }
    else
    {
        fread(data, width * height, size, f);
    }

    // Handle ABGR
    if (format == GL_BGRA)
    {
        unsigned int i;
        unsigned int *p = (unsigned int *)data;
//        for (i = width * height; i != 0; --i)
//        {
//            *p = swapUint32(*p);
//            p++;
//        }
    }

    fclose(f);

    return data;
}

}
