#include "pch.h"
#include "ArtemisColor.h"

nlohmann::json ArtemisColor::GetJson()
{
    nlohmann::json json;

    json["red"] = Red;
    json["green"] = Green;
    json["blue"] = Blue;

    return json;
}

void ArtemisColor::SetValues(LinearColor source)
{
    Red = source.R * 255.0;
    Green = source.G * 255.0;
    Blue = source.B * 255.0;
}
