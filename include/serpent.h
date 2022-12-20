#ifndef __SERPENT_H__
#define __SERPENT_H__

#include "entity.h"
#include "gf2d_sprite.h"
#include "gfc_list.h"
#include "gfc_audio.h"


typedef struct {
    int metabolism; //1-7, starts at 4.
    int lureStrength; //1-7, starts at 4.
    int stealth; //1-5, starts at 1.
    int speed; //1-5, starts at 1.
    int headStart; //1-3, starts at 1.

}SerpentPersStats;

typedef struct {
    int mutagen; //used for all modifications (common)
    int goldite; //used mostly for upgrades;
    int silverium; //used mostly for downgrades;
    int darkite; //used for high-level modifications (rare)
    int orbs; //used for revival chances (rare)
}PersCurrencies;

typedef struct {
    int score;
    float zoom;
    float health, healthMax;
    float levelUpDisplay;
    float size; //determines what can be eaten or scared
    float exp; //When it reaches or exceeds the Threshold, serpent levels up and grows.
    float expThreshold;
    int level;
    int length; //how many segments
    float hunger; //when it reaches 1, serpent loses health
    float hungerRate;
    int luring, snapping;
    List* snappables; //List of prey ready to be snapped.
    float lureOffset;
    Sprite* hpBar;
    int still;
    SerpentPersStats* persStats;
    PersCurrencies* currencies;
    int snap;
}SerpentData;
/**
 * @brief Create a new serpent entity
 * @param position where to spawn the serpent at
 * @return NULL on error, or an serpent entity pointer on success
 */
Entity* serpent_new(Vector3D position, SerpentPersStats* persStats, PersCurrencies* wallet);
Entity* serpent_spikeseg_new(Entity* par);
Entity* serpent_seg_new(Entity* par);

typedef enum {
    GM_COWARD,
    GM_NORMAL,
    GM_SASHIMI
} Mode;
typedef enum {
    Mutagen     = 0,
    Goldite     = 1,
    Silverium   = 2,
    Darkite     = 3,
    Orbs        = 4
} Currencies;
typedef enum {
    Metabolism      = 0,
    LureStrength    = 1,
    Stealth         = 2,
    Speed           = 3,
    HeadStart       = 4
} UpgrCats;

void setGameMode(Mode gm);
Mode getGameMode();

void levelUp(Entity* target);
void levelUpPart(Entity* target);

int hurtSerpent(Entity* serpent, Entity* attacker, float rawDmg);

int* getUpgradeCosts(UpgrCats category, int currentLevel, int direction);
int* getCanUpgrade(UpgrCats category, int currentLevel, int direction, PersCurrencies* wallet); //returns array of 1s on can buy, 0s for any insuffiecient amount.
void Upgrade(UpgrCats category, int currentLevel, int direction, PersCurrencies* wallet, SerpentPersStats* target);

void Save(PersCurrencies* wallet, SerpentPersStats* stats);


typedef enum {
    AB_NONE = 0,
    AB_PARALYSIS,
    AB_POISON,
    AB_INVISIBLE,
    AB_AUTOMETABOLIZE,
    AB_DARKVISION,
    AB_INTIMIDATE,
    AB_SP_LURE,
    AB_WHALE_CALL,
    AB_REGENERATE,
    AB_BOUNTY_BONUS
}Ability;

#endif