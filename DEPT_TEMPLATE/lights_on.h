#ifndef LIGHTS_ON_H
#define LIGHTS_ON_H

void InitLightsOn(); // initalizes lights on
void UpdateLightsOn(); // player controls
void DrawLightsOn(); // draw onto the screen
void UnloadLightsOn(); // unload to free memory
bool checkLightsOnWinStatus(); // win status

extern bool lightsOnActive;

#endif