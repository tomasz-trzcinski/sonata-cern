#include "alieventmanager.hpp"

#include <dirent.h>
#include <fstream>

#include <json.hpp>
using json = nlohmann::json;

AliEventManager::AliEventManager()
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("events")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			m_trackFiles.push_back(ent->d_name);
			//printf ("%s\n", ent->d_name);
		}
		closedir(dir);
	}
	else {
		perror("");
	}

	//We care only for real files, not "." and ".."
	m_trackFiles.erase(std::remove_if(m_trackFiles.begin(), m_trackFiles.end(),
		[](std::string str)
		{
			return str.size() < 3;
		}
		), m_trackFiles.end());

	m_currentFileID = 0;

	setupEvent();
}

AliEventManager::~AliEventManager()
{

}

void AliEventManager::nextEvent()
{
	m_currentFileID = (m_currentFileID + 1) % m_trackFiles.size();

	setupEvent();
}

void AliEventManager::prevEvent()
{
	m_currentFileID = (m_currentFileID - 1) % m_trackFiles.size();

	setupEvent();
}

void AliEventManager::setupEvent()
{
	m_tracks.clear();

	std::string eventName = m_trackFiles[m_currentFileID];

	std::cout << eventName << std::endl;

	std::fstream eventFile;
	eventFile.open("events/" + eventName, std::ios::in);

	if (!eventFile.is_open())
	{
		std::cerr << "Can't open event file file: " << eventName << "." << std::endl;
		return;
	}

	json jEventFile;

	eventFile >> jEventFile;

	eventFile.close();

	json fTracksArray = *jEventFile.find("fTracks");

	for (auto fTrackIter = fTracksArray.begin(); fTrackIter != fTracksArray.end(); ++fTrackIter)
	{
		json mmalTrack = *fTrackIter;

		auto polyXArray = *mmalTrack.find("fPolyX");
		auto polyYArray = *mmalTrack.find("fPolyY");
		auto polyZArray = *mmalTrack.find("fPolyZ");
		auto PID = *mmalTrack.find("fPID");

		AliTrack track;
		track.m_PID = PID;

		size_t pointCount = polyXArray.size();

		auto polyXIter = polyXArray.begin();
		auto polyYIter = polyYArray.begin();
		auto polyZIter = polyZArray.begin();

		for (size_t i = 0; i < pointCount; ++i)
		{
			track.m_pointList.push_back({ *polyXIter, *polyYIter, *polyZIter });

			++polyXIter;
			++polyYIter;
			++polyZIter;
		}

		m_tracks.push_back(track);
	}
	std::cout << (int)m_tracks.size() << std::endl;
}
