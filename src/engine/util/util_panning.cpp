#include "util_panning.h"

namespace Util
{
    void WorldCoordinatesToScreenCoordinates(float OffsetX, float OffsetY, float WorldCoordX, float WorldCoordY, float &ScreenCoordX, float &ScreenCoordY)
    {
        ScreenCoordX = (WorldCoordX - OffsetX);
        ScreenCoordY = (WorldCoordY - OffsetY);
    }

    void ScreenCoordinatesToWorldCoordinates(float OffsetX, float OffsetY, float ScreenCoordX, float ScreenCoordY, float &WorldCoordX, float &WorldCoordY)
    {
        WorldCoordX = (ScreenCoordX + OffsetX);
        WorldCoordY = (ScreenCoordY + OffsetY);
    }


}
