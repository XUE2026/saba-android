#ifndef GOBOT_CONVERTER_H
#define GOBOT_CONVERTER_H

#include <string>

bool VMDToGobotAction(const char* vmdPath, const char* gobotPath);
bool GobotActionToVMD(const char* gobotPath, const char* vmdPath);

#endif // GOBOT_CONVERTER_H