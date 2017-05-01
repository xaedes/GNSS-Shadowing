#pragma once

#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace gnssShadowing {
namespace common {

	/// \brief      Opens a cv::FileStorage object from the given file path.
	/// \param      filename  The path to the file to be openend.
	/// \return     An opened OpenCV file storage.
	inline
	cv::FileStorage openFileStorage(const std::string& filename)
	{
		cv::FileStorage fs(filename, cv::FileStorage::READ);
		if (!fs.isOpened())
			throw std::runtime_error("Error: Couldn't open cv FileStorage file: " + filename);
		return fs;
	}
	
	/// \brief      Looks up and returns the tag named \a settingName in the \a
	///             settingsFile.
	///
	/// \param      settingName   The name of the tag to be read.
	/// \param      settingsFile  The opened settings file to look the tag up in.
	///
	/// \return     The cv::FileNode if it exists.
	/// \throws     std::runtime_error  if the tag does not exist.
	inline
	cv::FileNode readNodeAndThrowIfNonexistent(const std::string& settingName, cv::FileStorage& settingsFile)
	{
		cv::FileNode node = settingsFile[settingName];
		if (node.empty())
			throw std::runtime_error("Error: Attempted to read nonexistent setting: " + settingName);
		return node;
	}
	
	/// \brief      Looks up and returns the value of the tag named \a
	///             settingName in the \a settingsFile, or \a defaultValue if it
	///             doesn't exist.
	///
	/// \param      settingName   The name of the tag to be read.
	/// \param      settingsFile  The opened settingsFile to look the tag up in.
	/// \param      defaultValue  The default value to be used tag can't be read.
	///
	/// \tparam     T             The type of the value to be read.
	///
	/// \return     The value of the tag, or \a default value if it doesn't
	///             exist.
	template <typename T>
	inline
	T readNodeAndDefaultIfNonexistent(const std::string& settingName, cv::FileStorage& settingsFile, T defaultValue)
	{
		cv::FileNode node = settingsFile[settingName];
		if (node.empty())
			return defaultValue;
		return (T)node;
	}
	
	/// \brief      Checks whether the tag named \a settingName is enabled or
	///             not. More precisely, the tag is read as numeric value and
	///             considered "enabled" if it is not equal to zero.
	///
	/// \param      settingName   The name of the tag to be read.
	/// \param      settingsFile  The opened settingsFile to look the tag up in.
	///
	/// \return     Whether the tag is enabled or not.
	/// \throws     std::runtime_error  if the tag does not exist.
	inline
	bool isSettingEnabled(const std::string& settingName, cv::FileStorage& settingsFile)
	{
		cv::FileNode node = readNodeAndThrowIfNonexistent(settingName, settingsFile);
		return (int)node == 0 ? false : true;
	}
	
	/// \brief      Looks up and returns the string value of the tag named \a
	///             settingName.
	///
	/// \param      settingName   The name of the tag to be read.
	/// \param      settingsFile  The opened settingsFile to look the tag up in.
	///
	/// \return     The string value of the tag.
	/// \throws     std::runtime_error  if the tag does not exist.
	inline
	std::string readSettingsString(const std::string& settingName, cv::FileStorage& settingsFile)
	{
		cv::FileNode node = readNodeAndThrowIfNonexistent(settingName, settingsFile);
		return (std::string)node;
	}
	
	/// \brief      Looks up and returns the list of strings in the tag named \a
	///             settingName.
	///
	/// \param      settingName   The name of the tag to be read.
	/// \param      settingsFile  The opened settingsFile to look the tag up in.
	///
	/// \return     A list of strings contained in the tag.
	/// \throws     std::runtime_error  if the tag does not exist.
	inline
	std::vector<std::string> readStringList(const std::string& settingName, cv::FileStorage& settingsFile)
	{
		std::vector<std::string> list;
		cv::FileNode node = readNodeAndThrowIfNonexistent(settingName, settingsFile);
		if (node.isSeq())
		{
			cv::FileNodeIterator it = node.begin(), it_end = node.end();
			for (; it != it_end; ++it)
				list.push_back((std::string)*it);
		}
		else
		{
			list.push_back((std::string)node);
		}
		return list;
	}
	
} // namespace common
} // namespace gnssShadowing
