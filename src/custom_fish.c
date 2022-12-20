#include "custom_fish.h"

#include "simple_json.h"
#include "simple_json_string.h"
#include "simple_json_parse.h"
#include "simple_json_error.h"
#include "simple_logger.h"

Entity* switchPart(Entity* oldPart, FishPart partType, int id) {
	Color col;
	if (oldPart) {
		col = oldPart->color;
		entity_free(oldPart);
	}
	else {
		col = gfc_color(1, 1, 1, 1);
	}
	Entity* part = NULL;
	part = entity_new();
	if (!part)
		return NULL;
	char model[100];
	switch (partType) {
	case P_BODY:
		snprintf(model, sizeof(model), "models/cBody.model");
		break;
	case P_HEAD:
		snprintf(model, sizeof(model), "models/cHead%i.model", id);
		break;
	case P_FINS:
		snprintf(model, sizeof(model), "models/cFin%i.model", id);
		break;
	case P_TAIL:
		snprintf(model, sizeof(model), "models/cTail%i.model", id);
		break;
	}
	part->model = gf3d_model_load(model);
	part->color = col;
	return part;
}
void switchPartColor(int colorID, Entity* part) {
	Color color = gfc_color(1, 1, 1, 1);

	switch (colorID) {
	case 1:
		color = gfc_color8(205, 34, 34, 255);
		break;
	case 2:
		color = gfc_color8(70, 150, 14, 255);
		break;
	case 3:
		color = gfc_color8(47, 72, 217, 255);
		break;
	case 4:
		color = gfc_color8(230, 176, 0, 255);
		break;
	case 5:
		color = gfc_color8(143, 86, 59, 255);
		break;
	case 6:
		color = gfc_color8(108, 73, 121, 255);
		break;
	case 7:
		color = gfc_color8(71, 71, 71, 255);
		break;
	case 8:
		color = gfc_color8(255, 255, 255, 255);
		break;
	}

	part->color = color;
}
void* randomizeFishInit(int* head, int* fins, int* tail, int* color) {
	*head = min((int)((float)rand() / (float)RAND_MAX * 3) + 1, 3);
	*fins = min((int)((float)rand() / (float)RAND_MAX * 3) + 1, 3);
	*tail = min((int)((float)rand() / (float)RAND_MAX * 3) + 1, 3);
	*color = min((int)((float)rand() / (float)RAND_MAX * 8) + 1, 3);
}

void SaveFish(int head, int fins, int tail, int color){
	SJson* saveFile = sj_load("customFish.sav");
	if (!saveFile) {
		slog("No fish file yet! Creating one now.");
		saveFile = sj_object_new();
		slog_sync();
	}
	SJson* jhead = sj_object_new();
	SJson* jfins = sj_object_new();
	SJson* jtail = sj_object_new();
	SJson* c = sj_object_new();
	jhead = sj_new_int(head);
	jfins = sj_new_int(fins);
	jtail = sj_new_int(tail);
	c = sj_new_int(color);

	sj_object_delete_key(jhead, "head");
	sj_object_delete_key(jfins, "fins");
	sj_object_delete_key(jtail, "tail");
	sj_object_delete_key(c, "c");

	sj_object_insert(saveFile, "head", jhead);
	sj_object_insert(saveFile, "fins", jfins);
	sj_object_insert(saveFile, "tail", jtail);
	sj_object_insert(saveFile, "c", c);

	sj_save(saveFile, "customFish.sav");

	sj_free(saveFile);
	sj_object_free(jhead);
	sj_object_free(jfins);
	sj_object_free(jtail);
	sj_object_free(c);

	slog("Fish Saved.");
	slog_sync();
}

void readCustomFish(Entity** body, Entity** head, Entity** fins, Entity** tail, int* hid, int* fid, int* tid, int* cid) {
	SJson* saveFile = sj_load("customFish.sav");
	if (!saveFile) {
		randomizeFishInit(hid, fid, tid, cid);
		slog("Generated random fish.");
	}
	else {
		SJson* jhead = sj_object_new();
		SJson* jfins = sj_object_new();
		SJson* jtail = sj_object_new();
		SJson* c = sj_object_new();
		jhead = sj_object_get_value(saveFile, "head");
		jfins = sj_object_get_value(saveFile, "fins");
		jtail = sj_object_get_value(saveFile, "tail");
		c = sj_object_get_value(saveFile, "c");
		sj_get_integer_value(jhead, hid);
		sj_get_integer_value(jfins, fid);
		sj_get_integer_value(jtail, tid);
		sj_get_integer_value(c, cid);

		sj_free(saveFile);
		sj_object_free(jhead);
		sj_object_free(jfins);
		sj_object_free(jtail);
		sj_object_free(c);
		slog("Loaded fish's save file.");
	}
	*body = switchPart(NULL, P_BODY, 0);
	*head = switchPart(NULL, P_HEAD, *hid);
	*fins = switchPart(NULL, P_FINS, *fid);
	*tail = switchPart(NULL, P_TAIL, *tid);
	slog_sync();

}