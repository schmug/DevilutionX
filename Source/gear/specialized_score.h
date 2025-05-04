/**
 * @file specialized_score.h
 *
 * Interface for specialized item category scoring systems.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "items.h"
#include "gear/gear_score.h"

namespace devilution {

/**
 * @brief Base class for specialized item scoring strategies
 */
class ItemScorer {
public:
    virtual ~ItemScorer() = default;
    
    /**
     * @brief Calculates a specialized score for an item
     * @param item The item to score
     * @return The specialized score
     */
    virtual float CalculateScore(const Item &item) const = 0;
    
    /**
     * @brief Gets a text explanation of the specialized scoring
     * @param item The item to explain
     * @return A text explanation of the specialized scoring
     */
    virtual std::string GetScoreExplanation(const Item &item) const = 0;
};

/**
 * @brief Specialized scorer for weapon items
 */
class WeaponScorer : public ItemScorer {
public:
    float CalculateScore(const Item &item) const override;
    std::string GetScoreExplanation(const Item &item) const override;
    
private:
    /**
     * @brief Calculates the DPS (Damage Per Second) for a weapon
     * @param item The weapon item
     * @return The calculated DPS
     */
    float CalculateDPS(const Item &item) const;
    
    /**
     * @brief Calculates the value of elemental damage
     * @param item The weapon item
     * @return The score contribution from elemental damage
     */
    float CalculateElementalDamageValue(const Item &item) const;
    
    /**
     * @brief Calculates the value of critical hit bonuses
     * @param item The weapon item
     * @return The score contribution from critical hit bonuses
     */
    float CalculateCriticalHitValue(const Item &item) const;
};

/**
 * @brief Specialized scorer for armor items
 */
class ArmorScorer : public ItemScorer {
public:
    float CalculateScore(const Item &item) const override;
    std::string GetScoreExplanation(const Item &item) const override;
    
private:
    /**
     * @brief Calculates the value of damage reduction
     * @param item The armor item
     * @return The score contribution from damage reduction
     */
    float CalculateDamageReductionValue(const Item &item) const;
    
    /**
     * @brief Calculates the value of resistances
     * @param item The armor item
     * @return The score contribution from resistances
     */
    float CalculateResistancesValue(const Item &item) const;
    
    /**
     * @brief Calculates the value of defensive stats
     * @param item The armor item
     * @return The score contribution from defensive stats
     */
    float CalculateDefensiveStatsValue(const Item &item) const;
};

/**
 * @brief Specialized scorer for jewelry items
 */
class JewelryScorer : public ItemScorer {
public:
    float CalculateScore(const Item &item) const override;
    std::string GetScoreExplanation(const Item &item) const override;
    
private:
    /**
     * @brief Calculates the value of utility effects
     * @param item The jewelry item
     * @return The score contribution from utility effects
     */
    float CalculateUtilityEffectsValue(const Item &item) const;
    
    /**
     * @brief Calculates the value of proc chances
     * @param item The jewelry item
     * @return The score contribution from proc chances
     */
    float CalculateProcChancesValue(const Item &item) const;
    
    /**
     * @brief Calculates the value of unique bonuses
     * @param item The jewelry item
     * @return The score contribution from unique bonuses
     */
    float CalculateUniqueBonusesValue(const Item &item) const;
};

/**
 * @brief Factory for creating specialized item scorers
 */
class ItemScorerFactory {
public:
    /**
     * @brief Creates a specialized scorer for an item
     * @param item The item to create a scorer for
     * @return A unique pointer to the appropriate scorer
     */
    static std::unique_ptr<ItemScorer> CreateScorer(const Item &item);
    
    /**
     * @brief Creates a specialized scorer for a gear category
     * @param category The gear category to create a scorer for
     * @return A unique pointer to the appropriate scorer
     */
    static std::unique_ptr<ItemScorer> CreateScorer(GearCategory category);
};

} // namespace devilution
