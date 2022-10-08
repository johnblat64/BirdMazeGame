#include "util_panning.h"

namespace Util
{
    v2d ImGuiOffsetCoordinatesToImGuiScreenCoordinates(float OffsetX, float OffsetY, float WorldCoordX, float WorldCoordY)
    {
        return v2d 
        {
            WorldCoordX - OffsetX,
            WorldCoordY - OffsetY
        };
    }


}
