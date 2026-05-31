//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_LOCALIZATIONMANAGER_H_
#define SABA_VIEWER_LOCALIZATIONMANAGER_H_

#include <string>
#include <unordered_map>

namespace saba
{
	class LocalizationManager
	{
	public:
		enum Language
		{
			English,
			Chinese,
			Japanese
		};

		static LocalizationManager& GetInstance();

		void SetLanguage(Language lang);
		std::string GetString(const std::string& key) const;
		Language GetLanguage() const;

	private:
		LocalizationManager();
		~LocalizationManager();
		LocalizationManager(const LocalizationManager&) = delete;
		LocalizationManager& operator=(const LocalizationManager&) = delete;

		void LoadStrings();

		using StringMap = std::unordered_map<std::string, std::string>;

		StringMap m_enStrings;
		StringMap m_zhStrings;
		StringMap m_jaStrings;
		Language m_currentLang;
	};
}

#endif // !SABA_VIEWER_LOCALIZATIONMANAGER_H_