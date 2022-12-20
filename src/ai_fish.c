
#include "simple_logger.h"
#include "ai_fish.h"
#include "gfc_input.h"
#include "serpent.h"
#include "simple_json.h"
#include "custom_fish.h"

void prey_think_standard(Entity* self);
void prey_think_predatory(Entity* self);
void checkForDespawn(Entity* self);
void part_think(Entity* self) {
	self->position = self->parent->position;
	self->rotation = self->parent->rotation;
	self->scale = self->parent->scale;
}

void customize(Entity* body) {
	int hid, fid, tid, cid;
	Entity* head, * fins, * tail, * dummy;
	readCustomFish(&dummy, &head, &fins, &tail, &hid, &fid, &tid, &cid);
	entity_free(dummy);
	switchPartColor(cid, body);
	switchPartColor(cid, head);
	switchPartColor(cid, fins);
	switchPartColor(cid, tail);
	body->childCount = 3;
	body->children[0] = head;
	body->children[1] = fins;
	body->children[2] = tail;
	head->parent = body;
	fins->parent = body;
	tail->parent = body;
	head->think = part_think;
	fins->think = part_think;
	tail->think = part_think;
}

Entity* spawn_new_around_serpent(Entity* serpent, float radius, int ticksElapsed) {
	float yaw = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	float pitch = ((float)rand() / (float)RAND_MAX) * GFC_HALF_PI - GFC_HALF_PI / 2;
	Vector3D spawnPos = serpent->position;
	float dx = sinf(yaw);
	float dy = -cosf(yaw);
	float dz = sinf(pitch);
	spawnPos.x -= dx * radius * 0.8 * ((SerpentData*)(serpent->customData))->size;
	spawnPos.y -= dy * radius * 0.8 * ((SerpentData*)(serpent->customData))->size;
	spawnPos.z += dz * radius * 0.8 * ((SerpentData*)(serpent->customData))->size;

	float difficultySeed = SDL_clamp((((SerpentData*)(serpent->customData))->size * 0.8f + ((float)ticksElapsed / 120000) / 2), 1, 20);
	slog("Spawning fish with difficulty seed: %f", difficultySeed);
	float sizemin, sizemax, predatorChance = 0;

	if (difficultySeed < 10) {
		sizemin = difficultySeed * 0.5f;
		sizemax = difficultySeed * 1;
	}
	else {

		sizemin = difficultySeed * 0.7f;
		sizemax = difficultySeed * 1.2f;
	}

	if (difficultySeed < 2.5 && getGameMode() != GM_SASHIMI) {
		predatorChance = 0;
	}
	else if (difficultySeed < 5) {
		predatorChance = 0.01 * difficultySeed * 2;
	}
	else {
		predatorChance = 0.25;
	}
	if (getGameMode() == GM_SASHIMI) {
		predatorChance *= 2;
		if (predatorChance > 0.45)
			predatorChance = 0.45;
	}
	else if (getGameMode == GM_COWARD) {
		predatorChance *= 0.5;
	}

	float pred = ((float)rand() / (float)RAND_MAX);

	if (pred < predatorChance) {
		sizemax *= 1.5;
		return switch_new(spawnPos, sizemin, sizemax, serpent);
	}

	return prey_new(spawnPos, sizemin, sizemax, serpent);
}

Entity* prey_new(Vector3D position, float sizeMin, float sizeMax, Entity* serpent) {
	Entity* fish = NULL;
	float size = ((float)rand() / (float)RAND_MAX) * (sizeMax - sizeMin) + sizeMin;
	FishIdentity species = UNDEF;
	float seed = ((float)rand() / (float)RAND_MAX) * 30;
	if (seed < 9)
		species = ODDFISH;
	else if (seed < 15)
		species = HOGFISH;
	else if (seed < 21)
		species = MOSSPRAWN;
	else if (seed < 27)
		species = ALBEYENO;
	else if (seed < 29)
		species = ORCA;
	else
		species = CUSTOM;
	fish = prey_new_from_file(species, position, size, serpent);
	fish->entityType = ET_PREY;
	fish->onFear = prey_flee;
	return fish;
}

Entity* prey_new_from_file(FishIdentity species, Vector3D position, float size, Entity* serpent) {
	SJson* json;
	int isCustom = 0;
	if (!species)return NULL;
	switch (species) {
	case ODDFISH:
		json = sj_load("config/fish/oddfish.cfg");
		break;
	case HOGFISH:
		json = sj_load("config/fish/hogfish.cfg");
		break;
	case MOSSPRAWN:
		json = sj_load("config/fish/mossprawn.cfg");
		break;
	case ALBEYENO:
		json = sj_load("config/fish/albeyeno.cfg");
		break;
	case CUSTOM:
		json = sj_load("config/fish/customFish.cfg");
		SJson* s = sj_load("customFish.sav");
		if (s) {
			sj_free(s);
			break;
		}
		sj_free;
	case ORCA:
		json = sj_load("config/fish/sharkPoacher.cfg");
		break;
	default:
		return NULL;
	}
	if (!json)return NULL;
	char* model = sj_get_string_value(sj_object_get_value(json, "model"));
	char* strat = sj_get_string_value(sj_object_get_value(json, "strat"));
	float sizeMult, bounds, alert, mSize, nutrition, speed, sprint, score, cM, cGS, cD, cDChance, cO, cOChance, cannibalThreshold, hungerRate, pursuitRange, lungeRange, huntRange;
	int eats;
	sj_get_float_value(sj_object_get_value(json, "realSizeMultiplier"), &sizeMult);
	sj_get_float_value(sj_object_get_value(json, "bounds"), &bounds);
	sj_get_float_value(sj_object_get_value(json, "alert"), &alert);
	sj_get_float_value(sj_object_get_value(json, "maxSize"), &mSize);
	sj_get_float_value(sj_object_get_value(json, "nutrition"), &nutrition);
	sj_get_float_value(sj_object_get_value(json, "speed"), &speed);
	sj_get_float_value(sj_object_get_value(json, "sprintMultiplier"), &sprint);
	sj_get_float_value(sj_object_get_value(json, "scoreMultiplier"), &score);
	sj_get_float_value(sj_object_get_value(json, "mutagen"), &cM);
	sj_get_float_value(sj_object_get_value(json, "gold_silv"), &cGS);
	sj_get_float_value(sj_object_get_value(json, "darkite"), &cD);
	sj_get_float_value(sj_object_get_value(json, "darkiteChance"), &cDChance);
	sj_get_float_value(sj_object_get_value(json, "orbs"), &cO);
	sj_get_float_value(sj_object_get_value(json, "orbChance"), &cOChance);
	sj_get_integer_value(sj_object_get_value(json, "eats"), &eats);
	sj_get_float_value(sj_object_get_value(json, "cannibalism"), &cannibalThreshold);
	sj_get_float_value(sj_object_get_value(json, "hungerRate"), &hungerRate);
	sj_get_float_value(sj_object_get_value(json, "pursuit"), &pursuitRange);
	sj_get_float_value(sj_object_get_value(json, "lunge"), &lungeRange);
	sj_get_float_value(sj_object_get_value(json, "huntingRadius"), &huntRange);

	sj_free(json);

	size = min(size, mSize);
	Entity* fish = NULL;
	fish = fish_new();
	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for a new fish!");
		return NULL;
	}

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->identity = species;
	pd->status = IDLE;
	pd->serpent = serpent;
	pd->serpentLure = serpent->children[2];
	pd->size = size;
	pd->nutrition = nutrition * size;
	pd->normSpeed = speed * size;
	pd->fullSpeed = sprint * pd->normSpeed;
	pd->eats = eats;
	pd->cannibalThreshold = cannibalThreshold;
	if (strat[0] == 'C') {
		pd->strategy = TGT_CHEMOSYNTHESIS;
	}
	if (strat[0] == 'L') {
		pd->strategy = TGT_LARGEST;
	}
	if (strat[0] == 'S') {
		pd->strategy = TGT_SMALLEST;
	}
	if (strat[0] == 'N') {
		pd->strategy = TGT_NUTRITION;
	}
	if (strat[0] == 'F') {
		pd->strategy = TGT_FIRSTSPOTTED;
	}

	pd->score = pd->nutrition * score;
	for (int i = 0; i < 5; i++) {
		pd->reward[i] = 0;
	}
	pd->reward[0] = (int)floorf(cM * size);
	float hasGoldite = ((float)rand() / (float)RAND_MAX) * 2 - 1;
	if (hasGoldite > 0) {
		pd->reward[1] = (int)floorf(size * cGS);
	}
	else {
		pd->reward[2] = (int)floorf(size * cGS);
	}
	if (cD) {
		float hasDarkite = ((float)rand() / (float)RAND_MAX);
		if (hasDarkite < cDChance) {
			pd->reward[3] = cD;
		}
	}
	if (cO) {
		float hasOrbs = ((float)rand() / (float)RAND_MAX);
		if (hasOrbs < cOChance) {
			pd->reward[4] = cO;
		}
	}
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	pd->snappable = 0;
	pd->hunted = 0;

	fish->model = gf3d_model_load(model);
	fish->think = prey_think_standard;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size * sizeMult;
	fish->scale = vector3d(realSize, realSize, realSize);

	fish->bounds = gfc_sphere(0, 0, 0, bounds * realSize);
	fish->alertBounds = gfc_sphere(0, 0, 0, alert * realSize);
	fish->huntingBounds = gfc_sphere(0, 0, 0, huntRange * realSize);
	pd->pursuitLimit = pursuitRange * realSize;
	pd->lungeThreshold = lungeRange * realSize;
	fish->onEncounteredFish = chaseFish;
	fish->onEncounteredSerpent = chaseSerpent;

	if (species == CUSTOM) {
		customize(fish);
	}

	return fish;
}

Entity* switch_new(Vector3D position, float sizeMin, float sizeMax, Entity* serpent) {
	Entity* fish = NULL;
	float size = ((float)rand() / (float)RAND_MAX) * (sizeMax - sizeMin) + sizeMin;
	FishIdentity species = UNDEF;
	float seed = ((float)rand() / (float)RAND_MAX) * 10;
	if (seed < 4)
		species = SEAHORNET;
	else if (seed < 7)
		species = BLOODSHARK;
	else
		species = FREAK;
	fish = switch_new_from_file(species, position, size, serpent);
	fish->entityType = ET_SWITCH;
	fish->onFear = prey_flee;
	slog("Switch Spawned!");
	return fish;
}

Entity* switch_new_from_file(FishIdentity species, Vector3D position, float size, Entity* serpent) {
	SJson* json;
	if (!species)return NULL;
	switch (species) {
	case SEAHORNET:
		json = sj_load("config/fish/seaHornet.cfg");
		break;
	case BLOODSHARK:
		json = sj_load("config/fish/bloodshark.cfg");
		break;
	case FREAK:
		json = sj_load("config/fish/freak.cfg");
		break;
	default:
		return NULL;
	}
	if (!json)return NULL;
	char* model = sj_get_string_value(sj_object_get_value(json, "model"));
	char* strat = sj_get_string_value(sj_object_get_value(json, "strat"));
	float sizeMult, bounds, alert, mSize, nutrition, speed, sprint, score, cM, cGS, cD, cDChance, cO, cOChance, cannibalThreshold, hungerRate, pursuitRange, lungeRange, huntRange, dmg;
	int eats;
	sj_get_float_value(sj_object_get_value(json, "realSizeMultiplier"), &sizeMult);
	sj_get_float_value(sj_object_get_value(json, "bounds"), &bounds);
	sj_get_float_value(sj_object_get_value(json, "alert"), &alert);
	sj_get_float_value(sj_object_get_value(json, "maxSize"), &mSize);
	sj_get_float_value(sj_object_get_value(json, "nutrition"), &nutrition);
	sj_get_float_value(sj_object_get_value(json, "speed"), &speed);
	sj_get_float_value(sj_object_get_value(json, "sprintMultiplier"), &sprint);
	sj_get_float_value(sj_object_get_value(json, "scoreMultiplier"), &score);
	sj_get_float_value(sj_object_get_value(json, "mutagen"), &cM);
	sj_get_float_value(sj_object_get_value(json, "gold_silv"), &cGS);
	sj_get_float_value(sj_object_get_value(json, "darkite"), &cD);
	sj_get_float_value(sj_object_get_value(json, "darkiteChance"), &cDChance);
	sj_get_float_value(sj_object_get_value(json, "orbs"), &cO);
	sj_get_float_value(sj_object_get_value(json, "orbChance"), &cOChance);
	sj_get_integer_value(sj_object_get_value(json, "eats"), &eats);
	sj_get_float_value(sj_object_get_value(json, "cannibalism"), &cannibalThreshold);
	sj_get_float_value(sj_object_get_value(json, "hungerRate"), &hungerRate);
	sj_get_float_value(sj_object_get_value(json, "pursuit"), &pursuitRange);
	sj_get_float_value(sj_object_get_value(json, "lunge"), &lungeRange);
	sj_get_float_value(sj_object_get_value(json, "huntingRadius"), &huntRange);
	sj_get_float_value(sj_object_get_value(json, "damage"), &dmg);

	sj_free(json);

	size = min(size, mSize);
	Entity* fish = NULL;
	fish = fish_new();
	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for a new fish!");
		return NULL;
	}

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->identity = species;
	pd->status = IDLE;
	pd->serpent = serpent;
	pd->serpentLure = serpent->children[2];
	pd->size = size;
	pd->nutrition = nutrition * size;
	pd->normSpeed = speed * size;
	pd->fullSpeed = sprint * pd->normSpeed;
	pd->eats = eats;
	pd->cannibalThreshold = cannibalThreshold;
	if (strat[0] == 'L') {
		pd->strategy = TGT_LARGEST;
	}
	if (strat[0] == 'S') {
		pd->strategy = TGT_SMALLEST;
	}
	if (strat[0] == 'N') {
		pd->strategy = TGT_NUTRITION;
	}
	if (strat[0] == 'F') {
		pd->strategy = TGT_FIRSTSPOTTED;
	}

	pd->score = pd->nutrition * score;
	for (int i = 0; i < 5; i++) {
		pd->reward[i] = 0;
	}
	pd->reward[0] = (int)floorf(cM * size);
	float hasGoldite = ((float)rand() / (float)RAND_MAX) * 2 - 1;
	if (hasGoldite > 0) {
		pd->reward[1] = (int)floorf(size * cGS);
		pd->reward[2] = (int)floorf(size * cGS / 2);
	}
	else {
		pd->reward[2] = (int)floorf(size * cGS);
		pd->reward[1] = (int)floorf(size * cGS / 2);
	}
	if (cD) {
		float hasDarkite = ((float)rand() / (float)RAND_MAX);
		if (hasDarkite < cDChance) {
			pd->reward[3] = cD;
		}
	}
	if (cO) {
		float hasOrbs = ((float)rand() / (float)RAND_MAX);
		if (hasOrbs < cOChance) {
			pd->reward[4] = cO;
		}
	}
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	pd->snappable = 0;
	pd->hunted = 0;

	pd->damage = dmg;

	fish->model = gf3d_model_load(model);
	fish->think = prey_think_predatory;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size * sizeMult;
	fish->scale = vector3d(realSize, realSize, realSize);

	fish->bounds = gfc_sphere(0, 0, 0, bounds * realSize);
	fish->alertBounds = gfc_sphere(0, 0, 0, alert * realSize);
	fish->huntingBounds = gfc_sphere(0, 0, 0, huntRange * realSize);
	pd->pursuitLimit = pursuitRange * realSize;
	pd->lungeThreshold = lungeRange * realSize;
	fish->onEncounteredFish = chaseFish;
	fish->onEncounteredSerpent = chaseSerpent;

	return fish;
}

void prey_think_standard(Entity* self) {
	if (!self) return;
	float maxXR = 15.0f * GFC_DEGTORAD;
	float minXR = -maxXR;
	PreyData* pd = ((PreyData*)(self->customData));
	switch (pd->status)
	{
	case IDLE:
		if (vector3d_equal(pd->targetDirection, self->rotation)) {
			pd->lastDirection = self->rotation;
			float randX = ((float)rand() / (float)RAND_MAX) * (maxXR - minXR) + minXR;
			float randZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
			pd->targetDirection = vector3d(randX, 0, randZ);
		}
		Vector3D angleDiff = vector3d(pd->targetDirection.x - pd->lastDirection.x, 0, pd->targetDirection.z - pd->lastDirection.z);
		self->rotation.x += angleDiff.x / 1500;
		self->rotation.z += angleDiff.z / 1500;
		if (fabs(pd->targetDirection.x - self->rotation.x) < 2.5f * GFC_DEGTORAD) {
			self->rotation.x = pd->targetDirection.x;
		}
		if (fabs(pd->targetDirection.z - self->rotation.z) < 2.5f * GFC_DEGTORAD) {
			self->rotation.z = pd->targetDirection.z;
		}
		/*
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);
		*/

		Vector3D d = vector3d(0, 0, 0);
		//vector3d_angle_vectors(vector3d(self->rotation.x * GFC_RADTODEG,self->rotation.y * GFC_RADTODEG,self->rotation.z * GFC_RADTODEG), &d, NULL, NULL);

		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * pd->normSpeed;
		self->position.y += dy * 0.015 * self->scale.y * pd->normSpeed;
		self->position.z += dz * 0.015 * self->scale.y * pd->normSpeed;
		//vector3d_copy(self->bounds, self->position);

		Sphere fishBounds = self->bounds, lureAura = pd->serpentLure->fearBounds;
		vector3d_add(fishBounds, fishBounds, self->position);
		vector3d_add(lureAura, lureAura, pd->serpentLure->position);
		if (((SerpentData*)(pd->serpent->customData))->luring && ((SerpentData*)(pd->serpent->customData))->size >= pd->size && gfc_sphere_overlap(fishBounds, lureAura)) {
			pd->status = ATTRACTED;
			pd->lastDirection = self->rotation;

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

			//Vector3D diff = vector3d(pd->serpentLure->position.x - self->position.x, pd->serpentLure->position.y - self->position.y, pd->serpentLure->position.z - self->position.z);

			vector3d_normalize(&diff);
			//vector3d_angles(diff, &pd->targetDirection);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;
			//slog("A fish is attracted.");
			slog_sync();
		}
		break;
	case ATTRACTED:{
		if(!pd->snappable){
			if (!((SerpentData*)(pd->serpent->customData))->luring) {
				pd->status = IDLE;
				return;
			}

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

			//Vector3D diff = vector3d(pd->serpentLure->position.x - self->position.x, pd->serpentLure->position.y - self->position.y, pd->serpentLure->position.z - self->position.z);

			vector3d_normalize(&diff);
			//vector3d_angles(diff, &pd->targetDirection);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;

			Vector3D d = vector3d(0,0,0);
			//vector3d_angle_vectors(vector3d(self->rotation.x * GFC_RADTODEG,self->rotation.y * GFC_RADTODEG,self->rotation.z * GFC_RADTODEG), &d, NULL, NULL);

			float dx = 0;
			float dy = 0;
			float dz = 0;

			dx = sinf(self->rotation.z);
			dy = -cosf(self->rotation.z);
			dz = sinf(self->rotation.x);

			self->position.x -= dx * 0.015 * self->scale.y * pd->normSpeed; //TODO: Fix flipped fish here.
			self->position.y -= dy * 0.015 * self->scale.y * pd->normSpeed;
			self->position.z += dz * 0.015 * self->scale.y * pd->normSpeed;
			//vector3d_copy(self->bounds, self->position);

			//slog("Fish is swimming at %f, %f, %f.", self->position.x, self->position.y, self->position.z);

			Sphere sb, lb;
			sb = self->bounds;
			lb = pd->serpentLure->bounds;
			vector3d_add(sb, self->bounds, self->position);
			vector3d_add(lb, pd->serpentLure->bounds, pd->serpentLure->position);

			if (gfc_sphere_overlap(sb, lb)) {
				gfc_list_append(((SerpentData*)(pd->serpent->customData))->snappables, self);
				pd->snappable = 1;
			}
		}
		break;
	}
	case ALERTED: {
		pd->fearTimer -= 1;
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * pd->fullSpeed;
		self->position.y += dy * 0.015 * self->scale.y * pd->fullSpeed;
		self->position.z += dz * 0.015 * self->scale.y * pd->fullSpeed;
		if (pd->fearTimer <= 0) {
			pd->status = IDLE;
		}
		break;
	}
	case HUNGRY: {
		//Check if there's a current target:
		if (!pd->target) {
			pd->status = IDLE;
			break;
		}
		//Check if the fish can be lured instead:
		Sphere fishBounds = self->bounds, lureAura = pd->serpentLure->fearBounds;
		vector3d_add(fishBounds, fishBounds, self->position);
		vector3d_add(lureAura, lureAura, pd->serpentLure->position);
		if (((SerpentData*)(pd->serpent->customData))->luring && ((SerpentData*)(pd->serpent->customData))->size >= pd->size && gfc_sphere_overlap(fishBounds, lureAura)) {
			pd->status = ATTRACTED;
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = 0;

			pd->lastDirection = self->rotation;

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);
			
			vector3d_normalize(&diff);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;
			break;
		}
		//Check if current target is out of reach:
		if (isOutOfReach(self, pd->target)) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunted = 0;
				((PreyData*)(pd->target->customData))->hunter = 0;
			}
			pd->target = 0;
			pd->status = IDLE;
			break;
		}

		//Check if already reached target:
		Sphere sb, tb;
		sb = self->bounds;
		tb = pd->target->bounds;
		vector3d_add(sb, self->bounds, self->position);
		vector3d_add(tb, pd->target->bounds, pd->target->position);

		if (gfc_sphere_overlap(sb, tb)) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				slog("OM NOM NOM!");
				fish_free(pd->target, (pd->target->entityType == ET_SWITCH) ? 1 : 0);
			}
			else {

			}
			pd->target = 0;
			pd->status = IDLE;
			break;
		}

		//Pursue target:
		Vector3D diff = vector3d(pd->target->position.x - self->position.x, pd->target->position.y - self->position.y, pd->target->position.z - self->position.z);
		vector3d_normalize(&diff);
		pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
		self->rotation = pd->targetDirection;

		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * ((canLunge(self, pd->target)) ? pd->fullSpeed : pd->normSpeed);
		self->position.y += dy * 0.015 * self->scale.y * ((canLunge(self, pd->target)) ? pd->fullSpeed : pd->normSpeed);
		self->position.z += dz * 0.015 * self->scale.y * ((canLunge(self, pd->target)) ? pd->fullSpeed : pd->normSpeed);


		break;
	}
	default:
		break;
	}
	checkForDespawn(self);
}

void prey_think_predatory(Entity* self) {
	if (!self) return;
	float maxXR = 15.0f * GFC_DEGTORAD;
	float minXR = -maxXR;
	PreyData* pd = ((PreyData*)(self->customData));
	switch (pd->status)
	{
	case IDLE:
		if (vector3d_equal(pd->targetDirection, self->rotation)) {
			pd->lastDirection = self->rotation;
			float randX = ((float)rand() / (float)RAND_MAX) * (maxXR - minXR) + minXR;
			float randZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
			pd->targetDirection = vector3d(randX, 0, randZ);
		}
		Vector3D angleDiff = vector3d(pd->targetDirection.x - pd->lastDirection.x, 0, pd->targetDirection.z - pd->lastDirection.z);
		self->rotation.x += angleDiff.x / 1500;
		self->rotation.z += angleDiff.z / 1500;
		if (fabs(pd->targetDirection.x - self->rotation.x) < 2.5f * GFC_DEGTORAD) {
			self->rotation.x = pd->targetDirection.x;
		}
		if (fabs(pd->targetDirection.z - self->rotation.z) < 2.5f * GFC_DEGTORAD) {
			self->rotation.z = pd->targetDirection.z;
		}
		/*
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);
		*/

		Vector3D d = vector3d(0, 0, 0);
		//vector3d_angle_vectors(vector3d(self->rotation.x * GFC_RADTODEG,self->rotation.y * GFC_RADTODEG,self->rotation.z * GFC_RADTODEG), &d, NULL, NULL);

		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * pd->normSpeed;
		self->position.y += dy * 0.015 * self->scale.y * pd->normSpeed;
		self->position.z += dz * 0.015 * self->scale.y * pd->normSpeed;
		//vector3d_copy(self->bounds, self->position);

		Sphere fishBounds = self->bounds, lureAura = pd->serpentLure->fearBounds;
		vector3d_add(fishBounds, fishBounds, self->position);
		vector3d_add(lureAura, lureAura, pd->serpentLure->position);
		if (((SerpentData*)(pd->serpent->customData))->luring && ((SerpentData*)(pd->serpent->customData))->size >= pd->size && gfc_sphere_overlap(fishBounds, lureAura)) {
			pd->status = ATTRACTED;
			pd->lastDirection = self->rotation;

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

			//Vector3D diff = vector3d(pd->serpentLure->position.x - self->position.x, pd->serpentLure->position.y - self->position.y, pd->serpentLure->position.z - self->position.z);

			vector3d_normalize(&diff);
			//vector3d_angles(diff, &pd->targetDirection);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;
			//slog("A fish is attracted.");
		}
		else if (((SerpentData*)(pd->serpent->customData))->luring && ((SerpentData*)(pd->serpent->customData))->size < pd->size && gfc_sphere_overlap(fishBounds, lureAura)) {
			chaseSerpent(self, pd->serpent->children[3]);
		}
		break;
	case ATTRACTED: {
		if (!pd->snappable) {
			if (!((SerpentData*)(pd->serpent->customData))->luring) {
				pd->status = IDLE;
				return;
			}

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

			//Vector3D diff = vector3d(pd->serpentLure->position.x - self->position.x, pd->serpentLure->position.y - self->position.y, pd->serpentLure->position.z - self->position.z);

			vector3d_normalize(&diff);
			//vector3d_angles(diff, &pd->targetDirection);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;

			Vector3D d = vector3d(0, 0, 0);
			//vector3d_angle_vectors(vector3d(self->rotation.x * GFC_RADTODEG,self->rotation.y * GFC_RADTODEG,self->rotation.z * GFC_RADTODEG), &d, NULL, NULL);

			float dx = 0;
			float dy = 0;
			float dz = 0;

			dx = sinf(self->rotation.z);
			dy = -cosf(self->rotation.z);
			dz = sinf(self->rotation.x);

			self->position.x -= dx * 0.015 * self->scale.y * pd->normSpeed; //TODO: Fix flipped fish here.
			self->position.y -= dy * 0.015 * self->scale.y * pd->normSpeed;
			self->position.z += dz * 0.015 * self->scale.y * pd->normSpeed;
			//vector3d_copy(self->bounds, self->position);

			//slog("Fish is swimming at %f, %f, %f.", self->position.x, self->position.y, self->position.z);

			Sphere sb, lb;
			sb = self->bounds;
			lb = pd->serpentLure->bounds;
			vector3d_add(sb, self->bounds, self->position);
			vector3d_add(lb, pd->serpentLure->bounds, pd->serpentLure->position);

			if (gfc_sphere_overlap(sb, lb)) {
				gfc_list_append(((SerpentData*)(pd->serpent->customData))->snappables, self);
				pd->snappable = 1;
			}
		}
		break;
	}
	case ALERTED: {
		pd->fearTimer -= 1;
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * pd->fullSpeed;
		self->position.y += dy * 0.015 * self->scale.y * pd->fullSpeed;
		self->position.z += dz * 0.015 * self->scale.y * pd->fullSpeed;
		if (pd->fearTimer <= 0) {
			pd->status = IDLE;
		}
		break;
	}
	case HUNGRY: {
		//Check if there's a current target:
		if (!pd->target) {
			pd->status = IDLE;
			break;
		}
		//Check if the fish can be lured instead:
		Sphere fishBounds = self->bounds, lureAura = pd->serpentLure->fearBounds;
		vector3d_add(fishBounds, fishBounds, self->position);
		vector3d_add(lureAura, lureAura, pd->serpentLure->position);
		if (((SerpentData*)(pd->serpent->customData))->luring && ((SerpentData*)(pd->serpent->customData))->size >= pd->size && gfc_sphere_overlap(fishBounds, lureAura)) {
			pd->status = ATTRACTED;
			pd->lastDirection = self->rotation;
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			} 
			pd->target = 0;

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			vector3d_normalize(&diff);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;
			break;
		}
		//Check if current target is out of reach:
		if (isOutOfReach(self, pd->target)) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunted = 0;
				((PreyData*)(pd->target->customData))->hunter = 0;
			}
			pd->target = 0;
			pd->status = IDLE;
			break;
		}

		//Check if already reached target:
		Sphere sb, tb;
		sb = self->bounds;
		tb = pd->target->bounds;
		vector3d_add(sb, self->bounds, self->position);
		vector3d_add(tb, pd->target->bounds, pd->target->position);

		if (gfc_sphere_overlap(sb, tb)) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				slog("OM NOM NOM!");
				fish_free(pd->target, (pd->target->entityType == ET_SWITCH) ? 1 : 0);
				pd->target = 0;
				pd->status = IDLE;
			}
			else {
				slog("CHOMP!");
				int killed = hurtSerpent(pd->target->rootParent, self, pd->damage * (pd->size - ((SerpentData*)(pd->serpent->customData))->size));
				if (!killed) {
					prey_flee(self, pd->target);
					pd->target = 0;
				}
				else {
					pd->target = 0;
					pd->status = IDLE;
				}
			}
			break;
		}

		//Pursue target:
		Vector3D diff = vector3d(pd->target->position.x - self->position.x, pd->target->position.y - self->position.y, pd->target->position.z - self->position.z);
		vector3d_normalize(&diff);
		pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
		self->rotation = pd->targetDirection;

		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * ((canLunge(self, pd->target)) ? pd->fullSpeed : pd->normSpeed);
		self->position.y += dy * 0.015 * self->scale.y * ((canLunge(self, pd->target)) ? pd->fullSpeed : pd->normSpeed);
		self->position.z += dz * 0.015 * self->scale.y * ((canLunge(self, pd->target)) ? pd->fullSpeed : pd->normSpeed);


		break;
	}
	default:
		break;
	}
	checkForDespawn(self);
}

void prey_flee(Entity* self, Entity* scarer) {
	if (!self || !scarer) return;
	PreyData* pd = ((PreyData*)(self->customData));
	if ((((SerpentData*)(pd->serpent->customData))->luring && pd->status == ATTRACTED) || pd->snappable) return;
	pd->status = ALERTED;
	pd->lastDirection = self->rotation;

	//Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

	//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

	Vector3D diff = vector3d(scarer->position.x - self->position.x, scarer->position.y - self->position.y, scarer->position.z - self->position.z);

	vector3d_normalize(&diff);
	//vector3d_angles(diff, &pd->targetDirection);

	pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
	self->rotation = pd->targetDirection;

	pd->fearTimer = 4000;
}
void chaseSerpent(Entity* self, Entity* target) {
	if (!self || !target) return;
	PreyData* pd = ((PreyData*)(self->customData));
	slog("Deciding whether to hunt serpent...");
	slog_sync();
	switch (pd->strategy) {
	case TGT_FIRSTSPOTTED:
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
		}
		slog("Hunting serpent.");
		slog_sync();
		pd->status = HUNGRY;
		return;
	case TGT_LARGEST:
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
			slog("Hunting serpent.");
			slog_sync();
		}
		else if (pd->targetSize < ((SerpentData*)(target->rootParent->customData))->size) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
			slog("Hunting serpent.");
			slog_sync();
		}
		pd->status = HUNGRY;
		return;
	case TGT_SMALLEST:
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
			slog("Hunting serpent.");
			slog_sync();
		}
		else if (pd->targetSize > ((SerpentData*)(target->rootParent->customData))->size) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
			slog("Hunting serpent.");
			slog_sync();
		}
		pd->status = HUNGRY;
		return;
	case TGT_NUTRITION:
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
			slog("Hunting serpent.");
			slog_sync();
		}
		else if (pd->targetNutrition < 1000) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = target;
			pd->targetSize = ((SerpentData*)(target->rootParent->customData))->size;
			pd->targetNutrition = 1000;
			slog("Hunting serpent.");
			slog_sync();
		}
		pd->status = HUNGRY;
		return;
	}
}
void chaseFish(Entity* self, Entity* target) {
	if (!self || !target) return;
	PreyData* pd = ((PreyData*)(self->customData));
	if (((PreyData*)(target->customData))->hunted || ((PreyData*)(target->customData))->snappable) return;
	switch (pd->strategy) {
	case TGT_CHEMOSYNTHESIS:
		return;
	case TGT_FIRSTSPOTTED:
		slog("Considering to hunt.");
		if (cannibalInvalidityCheck(self, target))
			return;
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (FS)");
		}
		pd->status = HUNGRY;
		return;
	case TGT_LARGEST:
		slog("Considering to hunt.");
		if (cannibalInvalidityCheck(self, target))
			return;
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (LFS)");
		}
		else if (((PreyData*)(pd->target->customData))->size < ((PreyData*)(target->customData))->size) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (L)");
		}
		pd->status = HUNGRY;
		return;
	case TGT_SMALLEST:
		slog("Considering to hunt.");
		if (cannibalInvalidityCheck(self, target))
			return;
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (SFS)");
		}
		else if (((PreyData*)(pd->target->customData))->size > ((PreyData*)(target->customData))->size) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (S)");
		}
		pd->status = HUNGRY;
		return;
	case TGT_NUTRITION:
		slog("Considering to hunt.");
		if (cannibalInvalidityCheck(self, target))
			return;
		if (!pd->target) {
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (NFS)");
		}
		else if (pd->targetNutrition < ((PreyData*)(target->customData))->nutrition) {
			if (pd->target->entityType == ET_PREY || pd->target->entityType == ET_SWITCH) {
				((PreyData*)(pd->target->customData))->hunter = 0;
				((PreyData*)(pd->target->customData))->hunted = 0;
			}
			pd->target = target;
			pd->targetSize = ((PreyData*)(target->customData))->size;
			pd->targetNutrition = ((PreyData*)(target->customData))->nutrition;
			((PreyData*)(target->customData))->hunted = 1;
			((PreyData*)(target->customData))->hunter = self;
			slog("The fish hunts. (N)");
		}
		pd->status = HUNGRY;
		return;
	}
}
int cannibalInvalidityCheck(Entity* self, Entity* target) {
	if (!self || !target) return 1;
	PreyData* pd1 = ((PreyData*)(self->customData));
	PreyData* pd2 = ((PreyData*)(target->customData));

	if (pd2->size >= pd1->size) return 1;

	if (pd1->identity != pd2->identity) return 0;

	if (pd1->cannibalThreshold && pd1->cannibalThreshold <= pd1->size - pd2->size) {
		return 0;
	}
	return 1;
}

int isOutOfReach(Entity* self, Entity* target) {
	if (!self || !target) return 0;
	float diff = vector3d_magnitude_between(self->position, target->position);
	if (diff > ((PreyData*)(self->customData))->pursuitLimit) return 1;
	return 0;
}
int canLunge(Entity* self, Entity* target) {
	if (!self || !target) return 0;
	float diff = vector3d_magnitude_between(self->position, target->position);
	if (diff <= ((PreyData*)(self->customData))->lungeThreshold) return 1;
	return 0;
}

void onReachedLure(Entity* self) {
	if (!self) return;
	((PreyData*)(self->customData))->snappable = true;
	gfc_list_append(&((SerpentData*)(((PreyData*)(self->customData))->serpent->customData))->snappables, self);
}

void checkForDespawn(Entity* self) {
	if (!self)
		return;
	switch (self->entityType) {
	case ET_PREY: {
		PreyData* pd = ((PreyData*)(self->customData));
		float difference = vector3d_magnitude_between(self->position, pd->serpent->position);

		if (difference > 150 * ((SerpentData*)(pd->serpent->customData))->size * pd->size) {
			fish_free(self, 0);
		}

		break;
	}
	case ET_SWITCH:
	case ET_PREDATOR: {
		PreyData* pd = ((PreyData*)(self->customData));
		float difference = vector3d_magnitude_between(self->position, pd->serpent->position);

		if (difference > 200 * ((SerpentData*)(pd->serpent->customData))->size * pd->size) {
			fish_free(self, 1);
		}

		break;
	}
	}
}