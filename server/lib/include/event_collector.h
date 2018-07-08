#pragma once

#include "server.h"

class CEventCollector : public CServer
{
		enum class EState
		{
			Listening,
			ListenerExist,
			Capturing
		};

	public:
		CEventCollector();
		~CEventCollector();
		void StartCapture();
		void StopCapture();
		virtual void Update() override;

		//obtain data for drawing
		void* GetData() const {};

		virtual void OnNewListener() override;
		virtual void OnListenerDisonnected() override;


		//callbacks
		bool OnSampleEventInt(SSampleIntArg& arg);
		bool OnTimeIntervalEvent(STimeIntervalArg& arg);

		const std::vector<STimeIntervalArg>& GetIntervals() const;

	private:
		void GoToState(EState s);

	private:
		EState m_state {EState::Listening};
		std::vector<STimeIntervalArg> m_intervals;
};
