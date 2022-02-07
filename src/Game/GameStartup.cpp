#include "GameStartup.h"

#include <fstream>
#include <json.hpp>

Pong::GameStartupData::GameStartupData(std::string_view modJSONFilePath)
{
    std::ifstream stream(modJSONFilePath.data());

    nlohmann::json j;
  
    stream >> j;

    lhsPlayerHorizontalPos = j["lhsPlayerHorizontalPos"].get<float>();
    lhsPlayerScaling = glm::vec2(j["lhsPlayerScaling"][0].get<float>(), j["lhsPlayerScaling"][1].get<float>());
    lhsPlayerSpeed = j["lhsPlayerSpeed"].get<float>();

    rhsPlayerHorizontalPos = j["rhsPlayerHorizontalPos"].get<float>();
    rhsPlayerScaling = glm::vec2(j["rhsPlayerScaling"][0].get<float>(), j["rhsPlayerScaling"][1].get<float>());
    rhsPlayerSpeed = j["rhsPlayerSpeed"].get<float>();

    ballScaling = glm::vec2(j["ballScaling"][0].get<float>(), j["ballScaling"][1].get<float>());
    ballSpeed = j["ballSpeed"].get<float>();

    textures[0].Load(std::string_view(j["backgroundTexturePath"].get<std::string>()));
    textures[1].Load(std::string_view(j["lhsPlayerTexturePath"].get<std::string>()));
    textures[2].Load(std::string_view(j["rhsPlayerTexturePath"].get<std::string>()));
    textures[3].Load(std::string_view(j["ballTexturePath"].get<std::string>()));

    framesInFlight = j["framesInFlight"].get<uint32_t>();
}
