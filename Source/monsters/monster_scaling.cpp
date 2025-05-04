/**
 * @file monster_scaling.cpp
 *
 * Implementation of the monster stat scaling system that adjusts monster statistics based on player gear level.
 */
#include "monsters/monster_scaling.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#include "utils/log.hpp"
#include "utils/file_util.h"
#include "utils/parse_json.hpp"

namespace devilution {

MonsterStatScaler& MonsterStatScaler::GetInstance()
{
    static MonsterStatScaler instance;
    return instance;
}

void MonsterStatScaler::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Load default monster stat definitions
    LoadMonsterStatDefinitions("assets/txtdata/monsters/monster_scaling.json");
    
    // Register default custom scaling functions
    // (None by default, but this is where they would be registered)
    
    initialized = true;
    LogVerbose("Monster Stat Scaler initialized");
}

bool MonsterStatScaler::LoadMonsterStatDefinitions(const std::string &filePath)
{
    // Clear existing definitions
    monsterStatDefinitions.clear();
    
    // Try to load the file
    std::string jsonContent;
    if (!LoadFileIntoString(filePath, jsonContent)) {
        LogError("Failed to load monster stat definitions from {}", filePath);
        return false;
    }
    
    // Parse the JSON
    json_error_t error;
    json_t *root = json_loads(jsonContent.c_str(), 0, &error);
    if (!root) {
        LogError("Failed to parse monster stat definitions: {}", error.text);
        return false;
    }
    
    // Parse the monster definitions
    json_t *definitions = json_object_get(root, "monsterDefinitions");
    if (!json_is_array(definitions)) {
        LogError("Monster definitions is not an array");
        json_decref(root);
        return false;
    }
    
    // Iterate through the definitions
    size_t index;
    json_t *definition;
    json_array_foreach(definitions, index, definition) {
        // Parse monster type
        json_t *typeJson = json_object_get(definition, "type");
        if (!json_is_integer(typeJson)) {
            LogError("Monster type is not an integer");
            continue;
        }
        monster_id type = static_cast<monster_id>(json_integer_value(typeJson));
        
        // Create a new definition
        MonsterStatDefinition statDef;
        statDef.type = type;
        
        // Parse level
        json_t *levelJson = json_object_get(definition, "level");
        if (json_is_integer(levelJson)) {
            statDef.level = json_integer_value(levelJson);
        }
        
        // Parse base stats
        json_t *baseStatsJson = json_object_get(definition, "baseStats");
        if (json_is_object(baseStatsJson)) {
            // Hit points
            json_t *hitPointsJson = json_object_get(baseStatsJson, "hitPoints");
            if (json_is_integer(hitPointsJson)) {
                statDef.baseHitPoints = json_integer_value(hitPointsJson);
            }
            
            // Damage
            json_t *damageMinJson = json_object_get(baseStatsJson, "damageMin");
            if (json_is_integer(damageMinJson)) {
                statDef.baseDamageMin = json_integer_value(damageMinJson);
            }
            
            json_t *damageMaxJson = json_object_get(baseStatsJson, "damageMax");
            if (json_is_integer(damageMaxJson)) {
                statDef.baseDamageMax = json_integer_value(damageMaxJson);
            }
            
            // Armor class
            json_t *armorClassJson = json_object_get(baseStatsJson, "armorClass");
            if (json_is_integer(armorClassJson)) {
                statDef.baseArmorClass = json_integer_value(armorClassJson);
            }
            
            // To-hit chance
            json_t *toHitChanceJson = json_object_get(baseStatsJson, "toHitChance");
            if (json_is_integer(toHitChanceJson)) {
                statDef.baseToHitChance = json_integer_value(toHitChanceJson);
            }
            
            // Special effectiveness
            json_t *specialEffectivenessJson = json_object_get(baseStatsJson, "specialEffectiveness");
            if (json_is_real(specialEffectivenessJson)) {
                statDef.baseSpecialEffectiveness = json_real_value(specialEffectivenessJson);
            }
        }
        
        // Parse scaling parameters
        json_t *scalingJson = json_object_get(definition, "scaling");
        if (json_is_object(scalingJson)) {
            // Helper function to parse scaling parameters
            auto parseScalingParams = [](json_t *paramsJson, StatScalingParams &params) {
                if (!json_is_object(paramsJson)) {
                    return;
                }
                
                // Factor
                json_t *factorJson = json_object_get(paramsJson, "factor");
                if (json_is_real(factorJson)) {
                    params.scalingFactor = json_real_value(factorJson);
                }
                
                // Min value
                json_t *minJson = json_object_get(paramsJson, "min");
                if (json_is_real(minJson)) {
                    params.minValue = json_real_value(minJson);
                }
                
                // Max value
                json_t *maxJson = json_object_get(paramsJson, "max");
                if (json_is_real(maxJson)) {
                    params.maxValue = json_real_value(maxJson);
                }
                
                // Curve type
                json_t *curveJson = json_object_get(paramsJson, "curve");
                if (json_is_string(curveJson)) {
                    std::string curveStr = json_string_value(curveJson);
                    if (curveStr == "linear") {
                        params.curve = ScalingCurveType::Linear;
                    } else if (curveStr == "exponential") {
                        params.curve = ScalingCurveType::Exponential;
                    } else if (curveStr == "logarithmic") {
                        params.curve = ScalingCurveType::Logarithmic;
                    } else if (curveStr == "sigmoid") {
                        params.curve = ScalingCurveType::Sigmoid;
                    } else if (curveStr == "custom") {
                        params.curve = ScalingCurveType::Custom;
                    }
                }
            };
            
            // Hit points scaling
            json_t *hitPointsScalingJson = json_object_get(scalingJson, "hitPoints");
            parseScalingParams(hitPointsScalingJson, statDef.hitPointsScaling);
            
            // Damage scaling
            json_t *damageScalingJson = json_object_get(scalingJson, "damage");
            parseScalingParams(damageScalingJson, statDef.damageScaling);
            
            // Armor class scaling
            json_t *armorClassScalingJson = json_object_get(scalingJson, "armorClass");
            parseScalingParams(armorClassScalingJson, statDef.armorClassScaling);
            
            // To-hit chance scaling
            json_t *toHitChanceScalingJson = json_object_get(scalingJson, "toHitChance");
            parseScalingParams(toHitChanceScalingJson, statDef.toHitChanceScaling);
            
            // Special effectiveness scaling
            json_t *specialEffectivenessScalingJson = json_object_get(scalingJson, "specialEffectiveness");
            parseScalingParams(specialEffectivenessScalingJson, statDef.specialEffectivenessScaling);
        }
        
        // Add the definition to the map
        monsterStatDefinitions[type] = statDef;
    }
    
    // Clean up
    json_decref(root);
    
    LogVerbose("Loaded {} monster stat definitions from {}", monsterStatDefinitions.size(), filePath);
    return true;
}

const MonsterStatDefinition& MonsterStatScaler::GetMonsterStatDefinition(monster_id type) const
{
    // Try to find the definition
    auto it = monsterStatDefinitions.find(type);
    if (it != monsterStatDefinitions.end()) {
        return it->second;
    }
    
    // If not found, return a default definition
    static const MonsterStatDefinition defaultDefinition;
    return defaultDefinition;
}

int MonsterStatScaler::ScaleHitPoints(const MonsterData &monster, float gearLevel) const
{
    // Get the monster definition
    const MonsterStatDefinition &def = GetMonsterStatDefinition(monster.type);
    
    // Get the base hit points
    float baseHitPoints = def.baseHitPoints > 0 ? def.baseHitPoints : monster.hitPoints;
    
    // Apply scaling
    float scaledHitPoints = ApplyScaling(baseHitPoints, gearLevel, def.hitPointsScaling);
    
    // Return as integer
    return static_cast<int>(scaledHitPoints);
}

int MonsterStatScaler::ScaleDamage(const MonsterData &monster, float gearLevel, bool isMinDamage) const
{
    // Get the monster definition
    const MonsterStatDefinition &def = GetMonsterStatDefinition(monster.type);
    
    // Get the base damage
    float baseDamage = isMinDamage ? 
        (def.baseDamageMin > 0 ? def.baseDamageMin : monster.damageMin) :
        (def.baseDamageMax > 0 ? def.baseDamageMax : monster.damageMax);
    
    // Apply scaling
    float scaledDamage = ApplyScaling(baseDamage, gearLevel, def.damageScaling);
    
    // Return as integer
    return static_cast<int>(scaledDamage);
}

int MonsterStatScaler::ScaleArmorClass(const MonsterData &monster, float gearLevel) const
{
    // Get the monster definition
    const MonsterStatDefinition &def = GetMonsterStatDefinition(monster.type);
    
    // Get the base armor class
    float baseArmorClass = def.baseArmorClass > 0 ? def.baseArmorClass : monster.armorClass;
    
    // Apply scaling
    float scaledArmorClass = ApplyScaling(baseArmorClass, gearLevel, def.armorClassScaling);
    
    // Return as integer
    return static_cast<int>(scaledArmorClass);
}

int MonsterStatScaler::ScaleToHitChance(const MonsterData &monster, float gearLevel) const
{
    // Get the monster definition
    const MonsterStatDefinition &def = GetMonsterStatDefinition(monster.type);
    
    // Get the base to-hit chance
    float baseToHitChance = def.baseToHitChance > 0 ? def.baseToHitChance : monster.toHitChance;
    
    // Apply scaling
    float scaledToHitChance = ApplyScaling(baseToHitChance, gearLevel, def.toHitChanceScaling);
    
    // Return as integer
    return static_cast<int>(scaledToHitChance);
}

float MonsterStatScaler::ScaleSpecialEffectiveness(const MonsterData &monster, float gearLevel) const
{
    // Get the monster definition
    const MonsterStatDefinition &def = GetMonsterStatDefinition(monster.type);
    
    // Get the base special effectiveness
    float baseSpecialEffectiveness = def.baseSpecialEffectiveness > 0.0f ? def.baseSpecialEffectiveness : 1.0f;
    
    // Apply scaling
    return ApplyScaling(baseSpecialEffectiveness, gearLevel, def.specialEffectivenessScaling);
}

void MonsterStatScaler::RegisterCustomScalingFunction(ScalingCurveType curveType, CustomScalingFunction function)
{
    customScalingFunctions[curveType] = function;
}

std::string MonsterStatScaler::GetMonsterScalingExplanation(const MonsterData &monster, float gearLevel) const
{
    std::stringstream explanation;
    explanation << "Monster Scaling Explanation for " << monster.name << " (Gear Level: " << gearLevel << "):\n\n";
    
    // Get the monster definition
    const MonsterStatDefinition &def = GetMonsterStatDefinition(monster.type);
    
    // Hit points
    float baseHitPoints = def.baseHitPoints > 0 ? def.baseHitPoints : monster.hitPoints;
    float scaledHitPoints = ApplyScaling(baseHitPoints, gearLevel, def.hitPointsScaling);
    explanation << "Hit Points:\n";
    explanation << "- Base: " << baseHitPoints << "\n";
    explanation << "- Scaled: " << scaledHitPoints << "\n";
    explanation << "- Scaling Factor: " << def.hitPointsScaling.scalingFactor << "\n";
    explanation << "- Curve Type: ";
    switch (def.hitPointsScaling.curve) {
    case ScalingCurveType::Linear:
        explanation << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        explanation << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        explanation << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        explanation << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        explanation << "Custom\n";
        break;
    }
    
    // Damage
    float baseDamageMin = def.baseDamageMin > 0 ? def.baseDamageMin : monster.damageMin;
    float baseDamageMax = def.baseDamageMax > 0 ? def.baseDamageMax : monster.damageMax;
    float scaledDamageMin = ApplyScaling(baseDamageMin, gearLevel, def.damageScaling);
    float scaledDamageMax = ApplyScaling(baseDamageMax, gearLevel, def.damageScaling);
    explanation << "\nDamage:\n";
    explanation << "- Base: " << baseDamageMin << "-" << baseDamageMax << "\n";
    explanation << "- Scaled: " << scaledDamageMin << "-" << scaledDamageMax << "\n";
    explanation << "- Scaling Factor: " << def.damageScaling.scalingFactor << "\n";
    explanation << "- Curve Type: ";
    switch (def.damageScaling.curve) {
    case ScalingCurveType::Linear:
        explanation << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        explanation << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        explanation << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        explanation << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        explanation << "Custom\n";
        break;
    }
    
    // Armor class
    float baseArmorClass = def.baseArmorClass > 0 ? def.baseArmorClass : monster.armorClass;
    float scaledArmorClass = ApplyScaling(baseArmorClass, gearLevel, def.armorClassScaling);
    explanation << "\nArmor Class:\n";
    explanation << "- Base: " << baseArmorClass << "\n";
    explanation << "- Scaled: " << scaledArmorClass << "\n";
    explanation << "- Scaling Factor: " << def.armorClassScaling.scalingFactor << "\n";
    explanation << "- Curve Type: ";
    switch (def.armorClassScaling.curve) {
    case ScalingCurveType::Linear:
        explanation << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        explanation << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        explanation << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        explanation << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        explanation << "Custom\n";
        break;
    }
    
    // To-hit chance
    float baseToHitChance = def.baseToHitChance > 0 ? def.baseToHitChance : monster.toHitChance;
    float scaledToHitChance = ApplyScaling(baseToHitChance, gearLevel, def.toHitChanceScaling);
    explanation << "\nTo-Hit Chance:\n";
    explanation << "- Base: " << baseToHitChance << "\n";
    explanation << "- Scaled: " << scaledToHitChance << "\n";
    explanation << "- Scaling Factor: " << def.toHitChanceScaling.scalingFactor << "\n";
    explanation << "- Curve Type: ";
    switch (def.toHitChanceScaling.curve) {
    case ScalingCurveType::Linear:
        explanation << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        explanation << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        explanation << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        explanation << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        explanation << "Custom\n";
        break;
    }
    
    // Special effectiveness
    float baseSpecialEffectiveness = def.baseSpecialEffectiveness > 0.0f ? def.baseSpecialEffectiveness : 1.0f;
    float scaledSpecialEffectiveness = ApplyScaling(baseSpecialEffectiveness, gearLevel, def.specialEffectivenessScaling);
    explanation << "\nSpecial Effectiveness:\n";
    explanation << "- Base: " << baseSpecialEffectiveness << "\n";
    explanation << "- Scaled: " << scaledSpecialEffectiveness << "\n";
    explanation << "- Scaling Factor: " << def.specialEffectivenessScaling.scalingFactor << "\n";
    explanation << "- Curve Type: ";
    switch (def.specialEffectivenessScaling.curve) {
    case ScalingCurveType::Linear:
        explanation << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        explanation << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        explanation << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        explanation << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        explanation << "Custom\n";
        break;
    }
    
    return explanation.str();
}

float MonsterStatScaler::ApplyScaling(float baseStat, float gearLevel, const StatScalingParams &params) const
{
    // Apply the appropriate scaling function
    float scaledStat = 0.0f;
    
    switch (params.curve) {
    case ScalingCurveType::Linear:
        scaledStat = ApplyLinearScaling(baseStat, gearLevel, params);
        break;
    case ScalingCurveType::Exponential:
        scaledStat = ApplyExponentialScaling(baseStat, gearLevel, params);
        break;
    case ScalingCurveType::Logarithmic:
        scaledStat = ApplyLogarithmicScaling(baseStat, gearLevel, params);
        break;
    case ScalingCurveType::Sigmoid:
        scaledStat = ApplySigmoidScaling(baseStat, gearLevel, params);
        break;
    case ScalingCurveType::Custom:
        // Use custom scaling function if registered
        auto it = customScalingFunctions.find(ScalingCurveType::Custom);
        if (it != customScalingFunctions.end()) {
            scaledStat = it->second(baseStat, gearLevel, params);
        } else {
            // Fall back to linear scaling
            scaledStat = ApplyLinearScaling(baseStat, gearLevel, params);
        }
        break;
    }
    
    // Clamp to min/max
    return std::clamp(scaledStat, params.minValue, params.maxValue);
}

float MonsterStatScaler::ApplyLinearScaling(float baseStat, float gearLevel, const StatScalingParams &params) const
{
    // Linear scaling: base + (factor * gearLevel)
    return baseStat + (params.scalingFactor * gearLevel);
}

float MonsterStatScaler::ApplyExponentialScaling(float baseStat, float gearLevel, const StatScalingParams &params) const
{
    // Exponential scaling: base * (1 + factor * gearLevel)
    return baseStat * (1.0f + params.scalingFactor * gearLevel);
}

float MonsterStatScaler::ApplyLogarithmicScaling(float baseStat, float gearLevel, const StatScalingParams &params) const
{
    // Logarithmic scaling: base * (1 + factor * log(gearLevel))
    // Ensure gearLevel is at least 1 to avoid negative logarithms
    float safeGearLevel = std::max(1.0f, gearLevel);
    return baseStat * (1.0f + params.scalingFactor * std::log(safeGearLevel));
}

float MonsterStatScaler::ApplySigmoidScaling(float baseStat, float gearLevel, const StatScalingParams &params) const
{
    // Sigmoid scaling: base * (1 + factor * (2 / (1 + exp(-gearLevel * 0.1)) - 1))
    // This gives an S-shaped curve that approaches asymptotes at very low and very high gear levels
    float sigmoid = 2.0f / (1.0f + std::exp(-gearLevel * 0.1f)) - 1.0f;
    return baseStat * (1.0f + params.scalingFactor * sigmoid);
}

} // namespace devilution
