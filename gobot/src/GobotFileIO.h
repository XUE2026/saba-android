#ifndef GOBOT_FILE_IO_H
#define GOBOT_FILE_IO_H

#include "gobot/GobotTypes.h"
#include <vector>
#include <string>

bool WriteGobotAction(const char* path,
                      const GobotActionHeader& header,
                      const GobotBoneFrame* bones,
                      const GobotCameraFrame* cameras,
                      const GobotMorphFrame* morphs);

bool ReadGobotAction(const char* path,
                     GobotActionHeader& header,
                     std::vector<GobotBoneFrame>& bones,
                     std::vector<GobotCameraFrame>& cameras,
                     std::vector<GobotMorphFrame>& morphs);

bool WriteGobotScene(const char* path,
                     const GobotSceneHeader& header,
                     const GobotSceneModel* models,
                     const GobotSceneLight* lights,
                     const GobotSceneGround* grounds);

bool ReadGobotScene(const char* path,
                    GobotSceneHeader& header,
                    std::vector<GobotSceneModel>& models,
                    std::vector<GobotSceneLight>& lights,
                    std::vector<GobotSceneGround>& grounds);

bool WriteGobotProject(const char* path,
                       const GobotProjectHeader& header,
                       const GobotProjEntry* entries,
                       const void* settingsData);

bool ReadGobotProject(const char* path,
                      GobotProjectHeader& header,
                      std::vector<GobotProjEntry>& entries,
                      std::vector<uint8_t>& settingsData);

#endif // GOBOT_FILE_IO_H