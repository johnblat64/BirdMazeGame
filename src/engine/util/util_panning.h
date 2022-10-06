#pragma once

namespace Util
{
    void
    WorldCoordinatesToScreenCoordinates(float OffsetX, float OffsetY, float WorldCoordX, float WorldCoordY, float &ScreenCoordX, float &ScreenCoordY);

    void
    ScreenCoordinatesToWorldCoordinates(float OffsetX, float OffsetY, float ScreenCoordX, float ScreenCoordY, float &WorldCoordX, float &WorldCoordY);

}