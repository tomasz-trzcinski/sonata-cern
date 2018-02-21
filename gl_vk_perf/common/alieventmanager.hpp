#ifndef __H_ALIEVENTMGR__
#define __H_ALIEVENTMGR__

#include <vector>

#include <glm/vec3.hpp>

class AliEventManager
{
	public:
		struct AliTrack
		{
			int32_t m_PID;
			std::vector<glm::vec3> m_pointList;
		};

	public:
		std::vector<std::string> m_trackFiles;
		size_t m_currentFileID;

		std::vector<AliTrack> m_tracks;

		AliEventManager();
		~AliEventManager();

		void setupEvent();
		void nextEvent();
		void prevEvent();
};
#endif