//sdcard.h
#pragma once
void initSD();
void writeToSDCard();
bool loadGimbalSDData(const char* filename);
void updateMissionFile(String field, String value);


