// CalcScreenPlacement body; params sp_x..sp_vertAlign (retail inlines it at a few sites)
extern ScreenPlacementGlob spGlob;
float GetRealWidthFromVirtualWidth(float width);
float GetRealHeightFromVirtualHeight(float height);
float GetVirtualWidthFromRealWidth(float width);
float GetVirtualHeightFromRealHeight(float height);
#ifndef SP
#define SP (&spGlob)
#endif
    float scaled;

    switch (sp_horzAlign) {
    case 1:
        scaled = SP->scaleVirtualToReal[0] * *sp_x;
        *sp_x = scaled + SP->realViewableMin[0];
        *sp_w = GetRealWidthFromVirtualWidth(*sp_w);
        break;
    case 2:
        scaled = SP->scaleVirtualToReal[0] * *sp_x;
        *sp_x = scaled + SP->realViewportSize[0] * 0.5f;
        *sp_w = GetRealWidthFromVirtualWidth(*sp_w);
        break;
    case 7:
        scaled = SP->scaleVirtualToReal[0] * *sp_x;
        *sp_x = scaled + (SP->realViewableMax[0] + SP->realViewableMin[0]) * 0.5f;
        *sp_w = GetRealWidthFromVirtualWidth(*sp_w);
        break;
    case 3:
        scaled = SP->scaleVirtualToReal[0] * *sp_x;
        *sp_x = scaled + SP->realViewableMax[0];
        *sp_w = GetRealWidthFromVirtualWidth(*sp_w);
        break;
    case 4:
        *sp_x = SP->scaleVirtualToFull[0] * *sp_x;
        *sp_w = SP->scaleVirtualToFull[0] * *sp_w;
        break;
    case 6:
        scaled = SP->scaleRealToVirtual[0] * *sp_x;
        *sp_x = scaled + SP->virtualScreenOffsetX;
        *sp_w = GetVirtualWidthFromRealWidth(*sp_w);
        break;
    case 0:
        scaled = SP->scaleVirtualToReal[0] * *sp_x;
        *sp_x = scaled + SP->virtualScreenOffsetX + SP->subScreenLeft;
        *sp_w = GetRealWidthFromVirtualWidth(*sp_w);
        break;
    case 5:
        break;
    default:
        scaled = SP->scaleVirtualToReal[0] * *sp_x;
        *sp_x = scaled + SP->virtualScreenOffsetX + SP->subScreenLeft;
        *sp_w = GetRealWidthFromVirtualWidth(*sp_w);
        break;
    }

    switch (sp_vertAlign) {
    case 1:
        scaled = SP->scaleVirtualToReal[1] * *sp_y;
        *sp_y = scaled + SP->realViewableMin[1];
        *sp_h = GetRealHeightFromVirtualHeight(*sp_h);
        break;
    case 2:
        scaled = SP->scaleVirtualToReal[1] * *sp_y;
        *sp_y = scaled + SP->realViewportSize[1] * 0.5f;
        *sp_h = GetRealHeightFromVirtualHeight(*sp_h);
        break;
    case 7:
        scaled = SP->scaleVirtualToReal[1] * *sp_y;
        *sp_y = scaled + (SP->realViewableMax[1] + SP->realViewableMin[1]) * 0.5f;
        *sp_h = GetRealHeightFromVirtualHeight(*sp_h);
        break;
    case 3:
        scaled = SP->scaleVirtualToReal[1] * *sp_y;
        *sp_y = scaled + SP->realViewableMax[1];
        *sp_h = GetRealHeightFromVirtualHeight(*sp_h);
        break;
    case 4:
        *sp_y = SP->scaleVirtualToFull[1] * *sp_y;
        *sp_h = SP->scaleVirtualToFull[1] * *sp_h;
        break;
    case 6:
        *sp_y = SP->scaleRealToVirtual[1] * *sp_y;
        *sp_h = GetVirtualHeightFromRealHeight(*sp_h);
        break;
    case 0:
        *sp_y = SP->scaleVirtualToReal[1] * *sp_y;
        *sp_h = GetRealHeightFromVirtualHeight(*sp_h);
        break;
    case 5:
        break;
    default:
        *sp_y = SP->scaleVirtualToReal[1] * *sp_y;
        *sp_h = GetRealHeightFromVirtualHeight(*sp_h);
        break;
    }
