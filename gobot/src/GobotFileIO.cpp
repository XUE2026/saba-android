#include "GobotFileIO.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

static bool CheckMagic(const char* actual, const char* expected, size_t len) {
    return std::memcmp(actual, expected, len) == 0;
}

// ── GobotAction ──────────────────────────────────────────────────────────────

bool WriteGobotAction(const char* path,
                      const GobotActionHeader& header,
                      const GobotBoneFrame* bones,
                      const GobotCameraFrame* cameras,
                      const GobotMorphFrame* morphs) {
    FILE* fp = std::fopen(path, "wb");
    if (!fp) return false;

    bool ok = true;
    ok = ok && (std::fwrite(&header, sizeof(GobotActionHeader), 1, fp) == 1);

    if (ok && header.numBoneFrames > 0 && bones) {
        ok = ok && (std::fwrite(bones, sizeof(GobotBoneFrame), header.numBoneFrames, fp) == header.numBoneFrames);
    }
    if (ok && header.numCameraFrames > 0 && cameras) {
        ok = ok && (std::fwrite(cameras, sizeof(GobotCameraFrame), header.numCameraFrames, fp) == header.numCameraFrames);
    }
    if (ok && header.numMorphFrames > 0 && morphs) {
        ok = ok && (std::fwrite(morphs, sizeof(GobotMorphFrame), header.numMorphFrames, fp) == header.numMorphFrames);
    }

    std::fclose(fp);
    return ok;
}

bool ReadGobotAction(const char* path,
                     GobotActionHeader& header,
                     std::vector<GobotBoneFrame>& bones,
                     std::vector<GobotCameraFrame>& cameras,
                     std::vector<GobotMorphFrame>& morphs) {
    FILE* fp = std::fopen(path, "rb");
    if (!fp) return false;

    bool ok = true;

    // Read header
    ok = ok && (std::fread(&header, sizeof(GobotActionHeader), 1, fp) == 1);
    if (ok) {
        ok = CheckMagic(header.magic, "GBAF", 4);
    }

    // Read bone frames
    if (ok && header.numBoneFrames > 0) {
        bones.resize(header.numBoneFrames);
        ok = ok && (std::fread(bones.data(), sizeof(GobotBoneFrame), header.numBoneFrames, fp) == header.numBoneFrames);
    }

    // Read camera frames
    if (ok && header.numCameraFrames > 0) {
        cameras.resize(header.numCameraFrames);
        ok = ok && (std::fread(cameras.data(), sizeof(GobotCameraFrame), header.numCameraFrames, fp) == header.numCameraFrames);
    }

    // Read morph frames
    if (ok && header.numMorphFrames > 0) {
        morphs.resize(header.numMorphFrames);
        ok = ok && (std::fread(morphs.data(), sizeof(GobotMorphFrame), header.numMorphFrames, fp) == header.numMorphFrames);
    }

    std::fclose(fp);

    if (!ok) {
        bones.clear();
        cameras.clear();
        morphs.clear();
    }
    return ok;
}

// ── GobotScene ───────────────────────────────────────────────────────────────

bool WriteGobotScene(const char* path,
                     const GobotSceneHeader& header,
                     const GobotSceneModel* models,
                     const GobotSceneLight* lights,
                     const GobotSceneGround* grounds) {
    FILE* fp = std::fopen(path, "wb");
    if (!fp) return false;

    bool ok = true;
    ok = ok && (std::fwrite(&header, sizeof(GobotSceneHeader), 1, fp) == 1);

    if (ok && header.numModels > 0 && models) {
        ok = ok && (std::fwrite(models, sizeof(GobotSceneModel), header.numModels, fp) == header.numModels);
    }
    if (ok && header.numLights > 0 && lights) {
        ok = ok && (std::fwrite(lights, sizeof(GobotSceneLight), header.numLights, fp) == header.numLights);
    }
    if (ok && header.numGrounds > 0 && grounds) {
        ok = ok && (std::fwrite(grounds, sizeof(GobotSceneGround), header.numGrounds, fp) == header.numGrounds);
    }

    std::fclose(fp);
    return ok;
}

bool ReadGobotScene(const char* path,
                    GobotSceneHeader& header,
                    std::vector<GobotSceneModel>& models,
                    std::vector<GobotSceneLight>& lights,
                    std::vector<GobotSceneGround>& grounds) {
    FILE* fp = std::fopen(path, "rb");
    if (!fp) return false;

    bool ok = true;
    ok = ok && (std::fread(&header, sizeof(GobotSceneHeader), 1, fp) == 1);
    if (ok) {
        ok = CheckMagic(header.magic, "GBSF", 4);
    }

    if (ok && header.numModels > 0) {
        models.resize(header.numModels);
        ok = ok && (std::fread(models.data(), sizeof(GobotSceneModel), header.numModels, fp) == header.numModels);
    }
    if (ok && header.numLights > 0) {
        lights.resize(header.numLights);
        ok = ok && (std::fread(lights.data(), sizeof(GobotSceneLight), header.numLights, fp) == header.numLights);
    }
    if (ok && header.numGrounds > 0) {
        grounds.resize(header.numGrounds);
        ok = ok && (std::fread(grounds.data(), sizeof(GobotSceneGround), header.numGrounds, fp) == header.numGrounds);
    }

    std::fclose(fp);

    if (!ok) {
        models.clear();
        lights.clear();
        grounds.clear();
    }
    return ok;
}

// ── GobotProject ─────────────────────────────────────────────────────────────

bool WriteGobotProject(const char* path,
                       const GobotProjectHeader& header,
                       const GobotProjEntry* entries,
                       const void* settingsData) {
    FILE* fp = std::fopen(path, "wb");
    if (!fp) return false;

    bool ok = true;
    ok = ok && (std::fwrite(&header, sizeof(GobotProjectHeader), 1, fp) == 1);

    uint32_t totalEntries = header.numScenes + header.numActions;
    if (ok && totalEntries > 0 && entries) {
        ok = ok && (std::fwrite(entries, sizeof(GobotProjEntry), totalEntries, fp) == totalEntries);
    }
    if (ok && header.settingsSize > 0 && settingsData) {
        ok = ok && (std::fwrite(settingsData, 1, header.settingsSize, fp) == header.settingsSize);
    }

    std::fclose(fp);
    return ok;
}

bool ReadGobotProject(const char* path,
                      GobotProjectHeader& header,
                      std::vector<GobotProjEntry>& entries,
                      std::vector<uint8_t>& settingsData) {
    FILE* fp = std::fopen(path, "rb");
    if (!fp) return false;

    bool ok = true;
    ok = ok && (std::fread(&header, sizeof(GobotProjectHeader), 1, fp) == 1);
    if (ok) {
        ok = CheckMagic(header.magic, "GBPJ", 4);
    }

    uint32_t totalEntries = header.numScenes + header.numActions;
    if (ok && totalEntries > 0) {
        entries.resize(totalEntries);
        ok = ok && (std::fread(entries.data(), sizeof(GobotProjEntry), totalEntries, fp) == totalEntries);
    }
    if (ok && header.settingsSize > 0) {
        settingsData.resize(header.settingsSize);
        ok = ok && (std::fread(settingsData.data(), 1, header.settingsSize, fp) == header.settingsSize);
    }

    std::fclose(fp);

    if (!ok) {
        entries.clear();
        settingsData.clear();
    }
    return ok;
}