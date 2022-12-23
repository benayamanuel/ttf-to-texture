#include <stdio.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "lib/stb_image_write.h"
#include "lib/stb_truetype.h"

typedef unsigned char byte;
typedef unsigned int u32;
typedef int i32;
typedef float f32;

i32 main(i32 argc, char** argv) {
    FILE* handle = fopen("ttf/mago1.ttf", "rb");
    fseek(handle, 0, SEEK_END);
    i32 len = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    void* buf = malloc(len);
    fread(buf, 1, len, handle);

    stbtt_fontinfo fInfo;
    stbtt_InitFont(&fInfo, (unsigned char*)buf, 0);

    f32 fontSize = 64.0f;
    f32 scale = stbtt_ScaleForPixelHeight(&fInfo, fontSize);

    // ascent : highest point relative to baseline. descent : lowest point
    i32 ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fInfo, &ascent, &descent, &lineGap);

    ascent *= scale;
    descent *= scale;
    lineGap *= scale;

    i32 textureWidth = 500;
    i32 textureHeight = 500;

    byte* buffer = (byte*)calloc(textureHeight * textureHeight, 1);

    i32 advance, leftBearing;
    i32 left, right, top, bottom;
    u32 originX = 0;
    u32 rowOffset = 0;

    // Loop through ASCII 65 -> 127
    for(u32 i = 65; i < 127; i++) {
        stbtt_GetCodepointHMetrics(&fInfo, i, &advance, &leftBearing);
        stbtt_GetCodepointBitmapBox(&fInfo, i, scale, scale, &left, &bottom, &right, &top);

        // printf("char: %c top: %d bottom: %d\n", i, top, bottom);
        if(originX + (right - left) >= textureWidth) {
            // Descend if x + next width exceed texture width.
            rowOffset += ((ascent + abs(descent)) * textureWidth);
            originX = 0;
        }

        // Height offset per character. Ascent -> goes to the baseline (from the top). Bottom -> per character vertical offset
        u32 heightOffset = (ascent + bottom) * textureWidth;
        stbtt_MakeCodepointBitmap(&fInfo, buffer + (u32)roundf(leftBearing * scale) + originX + rowOffset + heightOffset, right - left, top - bottom, textureWidth, scale, scale, i);

        // Query additional x offset (kerning) if available.
        i32 kern = stbtt_GetCodepointKernAdvance(&fInfo, i, i + 1);
        originX += roundf(scale * advance) + roundf(kern * scale);
    }

    stbi_write_png("texture.png", textureWidth, textureHeight, 1, buffer, textureWidth);

    free(buffer);
    free(buf);
    fclose(handle);

    return 0;
}