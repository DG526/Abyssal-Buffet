#pragma once
#include "entity.h"
#include "ai_fish.h"


typedef enum {
	P_BODY,
	P_HEAD,
	P_FINS,
	P_TAIL
}FishPart;

Entity* switchPart(Entity* oldPart, FishPart partType, int id);
void readCustomFish(Entity** body, Entity** head, Entity** fins, Entity** tail, int* hid, int* fid, int* tid, int* cid);
void* randomizeFishInit(int* head, int* fins, int* tail, int* color);
void SaveFish(int head, int fins, int tail, int color);
void switchPartColor(int color, Entity* part);