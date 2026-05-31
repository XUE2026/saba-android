//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ModelPreloader.h"

#include <Saba/Model/MMD/PMXFile.h>
#include <Saba/Model/MMD/PMXModel.h>
#include <thread>

namespace saba
{
	ModelPreloader::ModelPreloader()
		: m_progress(0.0f)
		, m_complete(false)
		, m_success(false)
		, m_cancelled(false)
	{
	}

	ModelPreloader::~ModelPreloader()
	{
		Cancel();
	}

	void ModelPreloader::PreloadModel(const std::string& path)
	{
		Cancel();

		m_progress = 0.0f;
		m_complete = false;
		m_success = false;
		m_cancelled = false;

		m_future = std::async(std::launch::async, &ModelPreloader::LoadTask, this, path);
	}

	void ModelPreloader::LoadTask(const std::string& path)
	{
		m_progress = 0.1f;

		if (m_cancelled) return;

		PMXFile pmxFile;
		if (!pmxFile.Load(path))
		{
			m_progress = 1.0f;
			m_complete = true;
			m_success = false;
			if (m_callback)
			{
				m_callback(false);
			}
			return;
		}

		m_progress = 0.5f;

		if (m_cancelled) return;

		auto model = std::make_shared<PMXModel>();
		if (!model->Create(std::move(pmxFile)))
		{
			m_progress = 1.0f;
			m_complete = true;
			m_success = false;
			if (m_callback)
			{
				m_callback(false);
			}
			return;
		}

		m_progress = 1.0f;
		m_complete = true;
		m_success = true;

		if (m_callback)
		{
			m_callback(true);
		}
	}

	float ModelPreloader::GetProgress() const
	{
		return m_progress.load();
	}

	bool ModelPreloader::IsComplete() const
	{
		return m_complete.load();
	}

	bool ModelPreloader::WasSuccessful() const
	{
		return m_success.load();
	}

	void ModelPreloader::Cancel()
	{
		m_cancelled = true;
		if (m_future.valid())
		{
			m_future.wait();
		}
	}

	void ModelPreloader::SetCallback(std::function<void(bool)> callback)
	{
		m_callback = callback;
	}
}