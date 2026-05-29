//
// Copyright(c) 2016-2017 benikabocha.
// Copyright(c) 2026 xueyixuan2026.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_LOCALIZATIONMANAGER_H_
#define SABA_VIEWER_LOCALIZATIONMANAGER_H_

#include <string>
#include <map>

namespace saba
{
    class LocalizationManager
    {
    public:
        enum class Language
        {
            English,
            Chinese,
            Japanese,
        };

        static LocalizationManager& GetInstance();

        void SetLanguage(Language lang);
        Language GetLanguage() const;
        const std::string& GetString(const std::string& key) const;

    private:
        LocalizationManager();
        ~LocalizationManager() = default;
        LocalizationManager(const LocalizationManager&) = delete;
        LocalizationManager& operator=(const LocalizationManager&) = delete;

        void Initialize();

        Language m_currentLanguage;
        std::map<Language, std::map<std::string, std::string>> m_translations;
    };
}

#endif // !SABA_VIEWER_LOCALIZATIONMANAGER_H_