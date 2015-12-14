#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <vector>
#include <string>

namespace Utility
{

/**
 * Join two filenames together by a path separator.
 *
 * @param a First path
 * @param b Second path
 * @return Concatenated file path
 */
std::string PathJoin(const std::string &a, const std::string &b);

/**
 * Return basename of the filename.
 *
 * @param path Filename
 * @return Filename basename
 */
std::string Basename(const std::string &path);

/**
 * Return the file extension from the filename.
 *
 * @param path Filename
 * @return File extension
 */
std::string Extension(const std::string &path);

/**
 * Return the file path without the file extension.
 *
 * @param filename Filename
 * @return Filename without file extension
 */
std::string Basefilename(const std::string &filename);

/**
 * Replace all occurances of the given string with another string.
 *
 * @param str String to search
 * @param from String to replace
 * @param to String to substitute with
 * @return String with substitutions
 */
std::string ReplaceAll(std::string str, const std::string& from,
    const std::string& to);

/**
 * Return true if string ends with given string, false otherwise. Case
 * sensitive.
 *
 * @param str String to search
 * @param suffix String to find
 * @return True if found, False otherwise
 */
bool EndsWith(const std::string &str, const std::string &suffix);

/**
 * Return true if filename is a directory, false otherwise.
 *
 * @param path Filename
 * @return True if filename is a directory, false otherwise
 */
bool IsDirectory(const std::string &path);

/**
 * Get all files in a directory based on glob pattern.
 *
 * @param path Filename to search
 * @param wildcard Glob pattern
 * @return Return list of files
 */
std::vector<std::string> GetFiles(const std::string &path,
    const std::string &wildcard="*");

/**
 * Get all directories in a directory.
 *
 * @param path Filename to search
 * @return List of directories
 */
std::vector<std::string> GetDirectories(const std::string &path);

/**
 * Detach process and write our PID file.
 *
 * @param pidFile PID filename
 */
void Daemonize(const std::string &pidFile);

/**
 * Remove PID file on process termination.
 *
 * @param status Return code
 * @param arg PID filename
 */
void DaemonizeExit(int status, void* arg);

/**
 * Write content to file.
 *
 * @param path Filename to write to
 * @param content Content to write out
 * @return True if successfully wrote out content, false otherwise
 */
bool WriteFile(const std::string &path, const std::string &content);

/**
 * Remove file.
 *
 * @param path Filename to remove
 * @return True if file was deleted, false otherwise
 */
bool RemoveFile(const std::string &path);

};

#endif
