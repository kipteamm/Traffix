#ifndef TRAFFIX_ROADCONFIG_H
#define TRAFFIX_ROADCONFIG_H
#include <string>
#include <unordered_map>
#include <vector>


constexpr int CURVEPOINTS = 20;


enum MarkingType {
    NONE, SOLID, DASHED
};


struct LaneConfig {
    float widthMeters;
};


struct MarkingConfig {
    // Where is it located relative to the road center?
    float offsetFromRoadCenterMeters;
    MarkingType markingType;
};


struct RoadConfig {
    std::vector<LaneConfig> lanes;
    std::vector<MarkingConfig> markings;
};


inline std::unordered_map<std::string, RoadConfig> CONFIGURATIONS = {
    {"Single lane - One Directional", {{{3.4}}, {{-1.5, SOLID}, {1.5, SOLID}}}},
    {"Two lane - Bi Directional", {{{3.3}, {3.3}}, {{-3.1, SOLID}, {3.1, SOLID}, {0, DASHED}}}}
};


#endif //TRAFFIX_ROADCONFIG_H