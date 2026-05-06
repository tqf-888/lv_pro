/**
 * @file lv_bmp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_BMP

#if defined(LV_USE_SUNXIFB_G2D_BLIT) || defined(LV_USE_SUNXIFB_G2D_BLEND)
#include "../../../../../lv_drivers/display/sunximem.h"
#endif /* LV_USE_SUNXIFB_G2D_BLIT */

#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_fs_file_t f;
    unsigned int px_offset;
    int px_width;
    int px_height;
    unsigned int bpp;
#ifdef LV_SUPPORT_PICTURE_VIEWER
    uint32_t compression; //压缩类型
#endif
    int row_size_bytes;
} bmp_dsc_t;

#ifdef LV_SUPPORT_PICTURE_VIEWER
static uint16_t rgb565_flag = 0;
// 压缩类型定义
#define BI_RGB        0
#define BI_BITFIELDS  3

// 常用掩码定义
#define RGB565_R_MASK 0xF800
#define RGB565_G_MASK 0x07E0
#define RGB565_B_MASK 0x001F

typedef struct {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
} RGBQUAD;
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);


static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_bmp_init(void)
{
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a PNG image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    LV_UNUSED(decoder);

    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/

    /*If it's a BMP file...*/
    if(src_type == LV_IMG_SRC_FILE) {
        const char * fn = src;
        if(strcmp(lv_fs_get_ext(fn), "bmp") == 0) {              /*Check the extension*/
            /*Save the data in the header*/
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) return LV_RES_INV;
            uint8_t headers[54];

            lv_fs_read(&f, headers, 54, NULL);
            uint32_t w;
            uint32_t h;
            memcpy(&w, headers + 18, 4);
            memcpy(&h, headers + 22, 4);
            header->w = w;
            header->h = h;
            header->always_zero = 0;
            lv_fs_close(&f);
#if LV_COLOR_DEPTH == 32
            uint16_t bpp;
            memcpy(&bpp, headers + 28, 2);
#if defined(LV_USE_SUNXIFB_G2D_BLIT) || defined(LV_USE_SUNXIFB_G2D_BLEND)
            header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
#else
            header->cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
#endif /* LV_USE_SUNXIFB_G2D_BLIT */
#else
            header->cf = LV_IMG_CF_TRUE_COLOR;
#endif
            return LV_RES_OK;
        }
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(src_type == LV_IMG_SRC_VARIABLE) {
        return LV_RES_INV;
    }

    return LV_RES_INV;         /*If didn't succeeded earlier then it's an error*/
}

static void convert_format(uint8_t *conv_buf, uint8_t *bmp_buf, int bmp_bpp) {
    if (LV_COLOR_DEPTH == 32) {
        if (bmp_bpp == 32 || bmp_bpp == 24) {
            conv_buf[0] = bmp_buf[0];
            conv_buf[1] = bmp_buf[1];
            conv_buf[2] = bmp_buf[2];
            conv_buf[3] = 0xff;
        } else if (bmp_bpp == 16) {
            if (rgb565_flag) {
                uint16_t rgb565 = bmp_buf[1] << 8 | bmp_buf[0];
                uint8_t r = (rgb565 >> 11) & 0x1f;
                uint8_t g = (rgb565 >> 5)  & 0x3f;
                uint8_t b = rgb565 & 0x1f;
                conv_buf[0] = (b << 3) | (b >> 2);
                conv_buf[1] = (g << 2) | (g >> 4);
                conv_buf[2] = (r << 3) | (r >> 2);
                conv_buf[3] = 0xff;
            } else {
                uint16_t rgb555 = bmp_buf[1] << 8 | bmp_buf[0];
                uint8_t r = (rgb555 >> 10) & 0x1f;
                uint8_t g = (rgb555 >> 5)  & 0x1f;
                uint8_t b = rgb555 & 0x1f;
                conv_buf[0] = (b << 3) | (b >> 2);
                conv_buf[1] = (g << 3) | (g >> 2);
                conv_buf[2] = (r << 3) | (r >> 2);
                conv_buf[3] = 0xff;
            }
        }
    } else if (LV_COLOR_DEPTH == 16) {
        if (bmp_bpp == 16) {
            conv_buf[0] = bmp_buf[0];
            conv_buf[1] = bmp_buf[1];
        }
    }
}

/**
 * Open a PNG image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might use it)
 * @return pointer to the decoded image or `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    /*If it's a PNG file...*/
    if(dsc->src_type == LV_IMG_SRC_FILE) {
        const char * fn = dsc->src;

        if(strcmp(lv_fs_get_ext(fn), "bmp") != 0) {
            return LV_RES_INV;       /*Check the extension*/
        }

        bmp_dsc_t b;
        memset(&b, 0x00, sizeof(b));

        lv_fs_res_t res = lv_fs_open(&b.f, dsc->src, LV_FS_MODE_RD);
        if(res == LV_RES_OK) return LV_RES_INV;

        uint8_t header[54];
        lv_fs_read(&b.f, header, 54, NULL);

        if(0x42 != header[0] || 0x4d != header[1]) {
            lv_fs_close(&b.f);
            return LV_RES_INV;
        }

        memcpy(&b.px_offset, header + 10, 4);
        memcpy(&b.px_width, header + 18, 4);
        memcpy(&b.px_height, header + 22, 4);
        memcpy(&b.bpp, header + 28, 2);
#ifdef LV_SUPPORT_PICTURE_VIEWER
        memcpy(&b.compression, header + 30, 4);
#endif
        b.row_size_bytes = ((b.bpp * b.px_width + 31) / 32) * 4;

        bool color_depth_error = false;
        if(LV_COLOR_DEPTH == 32 && (b.bpp != 32 && b.bpp != 24)) {
#ifndef LV_SUPPORT_PICTURE_VIEWER
            LV_LOG_WARN("LV_COLOR_DEPTH == 32 but bpp is %d (should be 32 or 24)", b.bpp);
            color_depth_error = true;
#endif
        }
        else if(LV_COLOR_DEPTH == 16 && b.bpp != 16) {
            LV_LOG_WARN("LV_COLOR_DEPTH == 16 but bpp is %d (should be 16)", b.bpp);
            color_depth_error = true;
        }
        else if(LV_COLOR_DEPTH == 8 && b.bpp != 8) {
            LV_LOG_WARN("LV_COLOR_DEPTH == 8 but bpp is %d (should be 8)", b.bpp);
            color_depth_error = true;
        }

        if(color_depth_error) {
            dsc->error_msg = "Color depth mismatch";
            lv_fs_close(&b.f);
            return LV_RES_INV;
        }

        dsc->user_data = lv_mem_alloc(sizeof(bmp_dsc_t));
        LV_ASSERT_MALLOC(dsc->user_data);
        if(dsc->user_data == NULL) return LV_RES_INV;
        memcpy(dsc->user_data, &b, sizeof(b));

#if defined(LV_USE_SUNXIFB_G2D_BLIT) || defined(LV_USE_SUNXIFB_G2D_BLEND)
        int j, k, index, bpp_byte, read_byte, line_bytes, alloc_bytes;
        uint8_t buf[4], *img_data;

        if (b.bpp == 24) {
            /*RGB888 --> ARGB8888*/
            bpp_byte = 4;
            read_byte = 3;
            alloc_bytes = b.px_height * b.px_width * bpp_byte;
        } else {
            bpp_byte = b.bpp / 8;
            read_byte = bpp_byte;
            alloc_bytes = b.row_size_bytes * b.px_height;
        }

        /*Go back two lines*/
        line_bytes = b.px_width * bpp_byte * 2;

        /*BMP images are stored upside down*/
        index = alloc_bytes - b.px_width * bpp_byte;
        img_data = (uint8_t*) sunxifb_mem_alloc(alloc_bytes, "bmp_decoder_open");

        if (NULL == img_data) {
            dsc->img_data = NULL;
            return LV_RES_OK;
        }

        lv_fs_seek(&b.f, b.px_offset, LV_FS_SEEK_SET);

        for (j = 0; j < b.px_height; j++) {
            for (k = 0; k < b.px_width; k++) {
                lv_fs_read(&b.f, &img_data[index], read_byte, NULL);
                if (b.bpp == 24) {
                    img_data[index + 3] = 0xff;
                }
                index += bpp_byte;
            }
            index -= line_bytes;
        }

        sunxifb_mem_flush_cache(img_data, alloc_bytes);
        dsc->img_data = img_data;
#elif defined(LV_SUPPORT_PICTURE_VIEWER)
        int x = 0, y = 0, index = 0, bpp_byte =0, read_byte = 0;
        int line_bytes = 0, alloc_bytes = 0, stride = 0, padding = 0;
        uint8_t buf[4] = {0}, *img_data = NULL;

        if (LV_COLOR_DEPTH == 32) {
            bpp_byte = 4;
        } else if (LV_COLOR_DEPTH == 16) {
            bpp_byte = 2;
        } else {
            dsc->img_data = NULL;
            return LV_RES_OK;
        }

        read_byte = b.bpp / 8;
        alloc_bytes = b.px_height * b.px_width * bpp_byte;
        /*Go back two lines*/
        line_bytes = b.px_width * bpp_byte * 2;
        // 计算行填充
        if (b.bpp % 8 == 0) {
            // 整数字节色深（如 8/16/24/32 位）
            stride = (b.px_width * read_byte + 3) & ~3;
            padding = stride - b.px_width * read_byte;
        } else {
            // 非整数字节色深（如 1 位）
            stride = ((b.px_width + 31) / 32) * 4;
            padding = stride - ((b.px_width + 7) / 8);
        }

        if (b.bpp == 16) {
            if (b.compression == BI_BITFIELDS) {
                uint32_t masks;
                lv_fs_read(&b.f, masks, 3, NULL);
                if (masks == RGB565_R_MASK && masks == RGB565_G_MASK \
                        && masks == RGB565_B_MASK) {
                    rgb565_flag = 1;
                } else {
                    rgb565_flag = 0;
                }
            } else {
                uint8_t rgb16[2] = {0};
                lv_fs_seek(&b.f, b.px_offset, LV_FS_SEEK_SET);
                // 检测是否有像素使用最高位（红色通道的第五位）
                int hasHighBit = 0;
                for (int i = 0; i < b.px_width; i++) {
                    lv_fs_read(&b.f, rgb16, read_byte, NULL);
                    if (rgb16[1] & 0x80) { // 检查第15位是否为1
                        hasHighBit = 1;
                        break;
                    }
                }
                if (hasHighBit) {
                    rgb565_flag = 1;
                } else {
                    rgb565_flag = 0;
                }
            }
        }

        /*BMP images are stored upside down*/
        index = alloc_bytes - b.px_width * bpp_byte;
        img_data = lv_mem_alloc(alloc_bytes);
        LV_ASSERT_MALLOC(img_data);

        if (NULL == img_data) {
            dsc->img_data = NULL;
            return LV_RES_OK;
        }


        if (b.bpp == 8 || b.bpp == 1) {
            uint8_t row[stride];
            uint32_t color, index = 0;
            int cont = (b.bpp == 8) ? 256 : 2;
            RGBQUAD palette[cont];
            uint32_t argb_palette[cont];
            lv_fs_read(&b.f, palette, cont * sizeof(RGBQUAD), NULL);
            for (int i = 0; i < cont; i++) {
                argb_palette[i] = 0xff000000 | \
                                 (palette[i].rgbRed << 16) | \
                                 (palette[i].rgbGreen << 8) | \
                                 palette[i].rgbBlue;
            }
            lv_fs_seek(&b.f, b.px_offset, LV_FS_SEEK_SET);
            // 逐行读取像素数据
            for (y = b.px_height - 1; y >= 0; y--) {
                lv_fs_read(&b.f, row, stride, NULL);
                for (x = 0; x < b.px_width; x++) {
                    if (b.bpp == 8) {
                        color = argb_palette[row[x]];
                    } else {
                        int byte_idx = x / 8;
                        int bit_idx = 7 - (x % 8);
                        uint8_t bit = (row[byte_idx] >> bit_idx) & 0x01;
                        color = argb_palette[bit];
                    }
                    index = (y * b.px_width + x) * 4;
                    if (LV_COLOR_DEPTH == 32) {
                        img_data[index]     = color & 0xff;         // B
                        img_data[index + 1] = (color >> 8) & 0xff;  // G
                        img_data[index + 2] = (color >> 16) & 0xff; // R
                        img_data[index + 3] = 0xff;                 // Alpha
                    }
                }
                if (padding) {
                    lv_fs_seek(&b.f, padding, LV_FS_SEEK_CUR);
                }
            }
        } else {
            lv_fs_seek(&b.f, b.px_offset, LV_FS_SEEK_SET);
            for (y = 0; y < b.px_height; y++) {
                for (x = 0; x < b.px_width; x++) {
                    lv_fs_read(&b.f, buf, read_byte, NULL);
                    convert_format(&img_data[index], buf, b.bpp);
                    index += bpp_byte;
                }
                if (padding) {
                    lv_fs_seek(&b.f, padding, LV_FS_SEEK_CUR);
                }
                index -= line_bytes;
            }
        }

        dsc->img_data = img_data;
#else
        dsc->img_data = NULL;
#endif /* LV_USE_SUNXIFB_G2D_BLIT */
        return LV_RES_OK;
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        return LV_RES_INV;
    }

    return LV_RES_INV;    /*If not returned earlier then it failed*/
}


static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    LV_UNUSED(decoder);

    bmp_dsc_t * b = dsc->user_data;
    y = (b->px_height - 1) - y; /*BMP images are stored upside down*/
    uint32_t p = b->px_offset + b->row_size_bytes * y;
    p += x * (b->bpp / 8);
    lv_fs_seek(&b->f, p, LV_FS_SEEK_SET);
    lv_fs_read(&b->f, buf, len * (b->bpp / 8), NULL);

#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 1
    for(unsigned int i = 0; i < len * (b->bpp / 8); i += 2) {
        buf[i] = buf[i] ^ buf[i + 1];
        buf[i + 1] = buf[i] ^ buf[i + 1];
        buf[i] = buf[i] ^ buf[i + 1];
    }

#elif LV_COLOR_DEPTH == 32
    if(b->bpp == 32) {
        lv_coord_t i;
        for(i = 0; i < len; i++) {
            uint8_t b0 = buf[i * 4];
            uint8_t b1 = buf[i * 4 + 1];
            uint8_t b2 = buf[i * 4 + 2];
            uint8_t b3 = buf[i * 4 + 3];
            lv_color32_t * c = (lv_color32_t *)&buf[i * 4];
            c->ch.red = b2;
            c->ch.green = b1;
            c->ch.blue = b0;
            c->ch.alpha = b3;
        }
    }
    if(b->bpp == 24) {
        lv_coord_t i;

        for(i = len - 1; i >= 0; i--) {
            uint8_t * t = &buf[i * 3];
            lv_color32_t * c = (lv_color32_t *)&buf[i * 4];
            c->ch.red = t[2];
            c->ch.green = t[1];
            c->ch.blue = t[0];
            c->ch.alpha = 0xff;
        }
    }
#endif

    return LV_RES_OK;
}


/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    bmp_dsc_t * b = dsc->user_data;
    lv_fs_close(&b->f);
#if defined(LV_USE_SUNXIFB_G2D_BLIT) || defined(LV_USE_SUNXIFB_G2D_BLEND)
    sunxifb_mem_free((void**) &dsc->img_data, "bmp_decoder_close");
#elif defined(LV_SUPPORT_PICTURE_VIEWER)
    lv_mem_free(dsc->img_data);
#endif /* LV_USE_SUNXIFB_G2D_BLIT */
    lv_mem_free(dsc->user_data);

}

#endif /*LV_USE_BMP*/
