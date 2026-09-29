#include "common_types.h"
#include "imports.h"

extern unsigned char spGlob[];
#define SP ((struct ScreenPlacementGlob *)spGlob)

extern byte *cls_ptr_195ecac;

float GetRealWidthFromVirtualWidth(float width);
float GetRealHeightFromVirtualHeight(float height);
float GetVirtualWidthFromRealWidth(float width);
float GetVirtualHeightFromRealHeight(float height);
void CalcScreenX(float *x, int horzAlign);
void CalcScreenY(float *y, int vertAlign);
void CalcSplitScreenTextOffset(FontHandle font, float *y);
void SetScreenScaling(float safeAreaRatioHorizontal, float safeAreaRatioVertical, int viewportX, int viewportY, float viewportWidth, float viewportHeight);
void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);

float GetRealWidthFromVirtualWidth(float width)
{
    return SP->scaleVirtualToReal[0] * width;
}

float GetRealHeightFromVirtualHeight(float height)
{
    return SP->scaleVirtualToReal[1] * height;
}

float GetVirtualWidthFromRealWidth(float width)
{
    return SP->scaleRealToVirtual[0] * width;
}

float GetVirtualHeightFromRealHeight(float height)
{
    return SP->scaleRealToVirtual[1] * height;
}

void CalcScreenX(float *x, int horzAlign)
{
    switch (horzAlign) {
    case 1:
        *x = SP->scaleVirtualToReal[0] * *x + SP->realViewableMin[0];
        break;
    case 2:
        *x = SP->scaleVirtualToReal[0] * *x + SP->realViewportSize[0] * 0.5f;
        break;
    case 7:
        *x = SP->scaleVirtualToReal[0] * *x + (SP->realViewableMax[0] + SP->realViewableMin[0]) * 0.5f;
        break;
    case 3:
        *x = SP->scaleVirtualToReal[0] * *x + SP->realViewableMax[0];
        break;
    case 4:
        *x = SP->scaleVirtualToFull[0] * *x;
        break;
    case 6:
        *x = SP->scaleRealToVirtual[0] * *x + SP->virtualScreenOffsetX;
        break;
    case 0:
        *x = SP->scaleVirtualToReal[0] * *x + SP->virtualScreenOffsetX + SP->subScreenLeft;
        break;
    case 5:
        break;
    default:
        *x = SP->scaleVirtualToReal[0] * *x + SP->virtualScreenOffsetX + SP->subScreenLeft;
        break;
    }
}

void CalcScreenY(float *y, int vertAlign)
{
    switch (vertAlign) {
    case 1:
        *y = SP->scaleVirtualToReal[1] * *y + SP->realViewableMin[1];
        break;
    case 2:
        *y = SP->scaleVirtualToReal[1] * *y + SP->realViewportSize[1] * 0.5f;
        break;
    case 7:
        *y = SP->scaleVirtualToReal[1] * *y + (SP->realViewableMax[1] + SP->realViewableMin[1]) * 0.5f;
        break;
    case 3:
        *y = SP->scaleVirtualToReal[1] * *y + SP->realViewableMax[1];
        break;
    case 4:
        *y = SP->scaleVirtualToFull[1] * *y;
        break;
    case 6:
        *y = SP->scaleRealToVirtual[1] * *y;
        break;
    case 0:
        *y = SP->scaleVirtualToReal[1] * *y;
        break;
    case 5:
        break;
    default:
        *y = SP->scaleVirtualToReal[1] * *y;
        break;
    }
}

void CalcSplitScreenTextOffset(FontHandle font, float *y)
{

}

void SetScreenScaling(float safeAreaRatioHorizontal, float safeAreaRatioVertical,
                      int viewportX, int viewportY, float viewportWidth, float viewportHeight)
{
    float fViewportHeight = viewportHeight;
    float fViewportWidth = viewportWidth;
    float fViewportY = (float)viewportY;
    float fViewportX = (float)viewportX;

    SP->realViewportSize[0] = fViewportWidth;
    SP->realViewportSize[1] = fViewportHeight;

    byte *cls = cls_ptr_195ecac;
    float adjustedRealWidth = fViewportHeight / ((clientStatic_t *)(cls))->vidConfig.aspectRatioPixel * (4.0f / 3.0f);
    float horzAspectPixelDiff = fViewportWidth - adjustedRealWidth;
    float horzAspectScale = fViewportWidth / adjustedRealWidth;

    float displayWidth = (float)(((clientStatic_t *)(cls))->vidConfig.width);
    float displayHeight = (float)(((clientStatic_t *)(cls))->vidConfig.height);

    float unsafeHorz = (1.0f - safeAreaRatioHorizontal) * 0.5f * displayWidth;
    float unsafeVert = (1.0f - safeAreaRatioVertical) * 0.5f * displayHeight;

    float safeRight = displayWidth - unsafeHorz;
    float safeBottom = displayHeight - unsafeVert;

    float safeLeft = (fViewportX - unsafeHorz >= 0) ? fViewportX : unsafeHorz;
    float safeTop = (fViewportY - unsafeVert >= 0) ? fViewportY : unsafeVert;
    float clampedRight = (safeRight - (fViewportX + fViewportWidth) >= 0) ? (fViewportX + fViewportWidth) : safeRight;
    float clampedBottom = (safeBottom - (fViewportY + fViewportHeight) >= 0) ? (fViewportY + fViewportHeight) : safeBottom;

    SP->realViewableMin[0] = safeLeft - fViewportX;
    SP->realViewableMin[1] = safeTop - fViewportY;
    SP->realViewableMax[0] = clampedRight - fViewportX;
    SP->realViewableMax[1] = clampedBottom - fViewportY;

    SP->virtualViewableMin[0] = horzAspectScale * SP->realViewableMin[0] * (640.0f / fViewportWidth);
    SP->virtualViewableMin[1] = SP->realViewableMin[1] * (480.0f / fViewportHeight);
    SP->virtualViewableMax[0] = horzAspectScale * SP->realViewableMax[0] * (640.0f / fViewportWidth);
    SP->virtualViewableMax[1] = SP->realViewableMax[1] * (480.0f / fViewportHeight);

    float scaleY = fViewportHeight / 480.0f;

    SP->scaleVirtualToReal[0] = adjustedRealWidth / 640.0f;
    SP->scaleVirtualToReal[1] = scaleY;

    SP->scaleVirtualToFull[0] = fViewportWidth / 640.0f;
    SP->scaleVirtualToFull[1] = scaleY;

    SP->scaleRealToVirtual[0] = 640.0f / adjustedRealWidth;
    SP->scaleRealToVirtual[1] = 480.0f / fViewportHeight;

    SP->subScreenLeft = 0.5f * horzAspectPixelDiff;
    SP->virtualScreenOffsetX = 640.0f - adjustedRealWidth * SP->scaleRealToVirtual[0];
}

void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign)
{
    switch (horzAlign) {
    case 1:
        *x = SP->scaleVirtualToReal[0] * *x + SP->realViewableMin[0];
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 2:
        *x = SP->scaleVirtualToReal[0] * *x + SP->realViewportSize[0] * 0.5f;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 7:
        *x = SP->scaleVirtualToReal[0] * *x + (SP->realViewableMax[0] + SP->realViewableMin[0]) * 0.5f;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 3:
        *x = SP->scaleVirtualToReal[0] * *x + SP->realViewableMax[0];
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 4:
        *x = SP->scaleVirtualToFull[0] * *x;
        *w = SP->scaleVirtualToFull[0] * *w;
        break;
    case 6:
        *x = SP->scaleRealToVirtual[0] * *x + SP->virtualScreenOffsetX;
        *w = SP->scaleRealToVirtual[0] * *w;
        break;
    case 0:
        *x = SP->scaleVirtualToReal[0] * *x + SP->virtualScreenOffsetX + SP->subScreenLeft;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 5:
        break;
    default:
        *x = SP->scaleVirtualToReal[0] * *x + SP->virtualScreenOffsetX + SP->subScreenLeft;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    }

    switch (vertAlign) {
    case 1:
        *y = SP->scaleVirtualToReal[1] * *y + SP->realViewableMin[1];
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 2:
        *y = SP->scaleVirtualToReal[1] * *y + SP->realViewportSize[1] * 0.5f;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 7:
        *y = SP->scaleVirtualToReal[1] * *y + (SP->realViewableMax[1] + SP->realViewableMin[1]) * 0.5f;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 3:
        *y = SP->scaleVirtualToReal[1] * *y + SP->realViewableMax[1];
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 4:
        *y = SP->scaleVirtualToFull[1] * *y;
        *h = SP->scaleVirtualToFull[1] * *h;
        break;
    case 6:
        *y = SP->scaleRealToVirtual[1] * *y;
        *h = SP->scaleRealToVirtual[1] * *h;
        break;
    case 0:
        *y = SP->scaleVirtualToReal[1] * *y;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 5:
        break;
    default:
        *y = SP->scaleVirtualToReal[1] * *y;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    }
}
