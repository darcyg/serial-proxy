#include "utility.hpp"

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <logcpp/logger.hpp>
using namespace logcpp;

namespace Utility
{

string Basename(const string &path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

string PathJoin(const string &a, const string &b)
{
    return a + "/" + b;
}

bool IsDirectory(const string &path)
{
    struct stat info;

    if(stat(path.c_str(), &info) != 0)
        return false;

    return info.st_mode & S_IFDIR;
}

string Extension(const string &path)
{
    return path.substr(path.find_last_of(".") + 1);
}

string Basefilename(const string &file)
{
    return file.substr(0, file.find_last_of("."));
}

bool EndsWith(const string &str, const string &suffix)
{
    if (str.length() >= suffix.length())
        return (0 == str.compare (str.length() - suffix.length(),
                                  suffix.length(), suffix));
    else
        return false;
}

string ReplaceAll(string str, const string& from, const string& to)
{
    if(from.empty())
        return str;

    size_t start_pos = 0;

    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}

vector<string> GetFiles(const string &path, const string &wildcard)
{
    vector<string> results;
    glob_t glob_result;

    glob((path + "/" + wildcard).c_str(), GLOB_TILDE, NULL, &glob_result);

    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        if (!IsDirectory(glob_result.gl_pathv[i]))
            results.push_back(glob_result.gl_pathv[i]);
    }

    globfree(&glob_result);

    return results;
}

vector<string> GetDirectories(const string &path)
{
    vector<string> results;
    glob_t glob_result;

    glob((path + "/*").c_str(), GLOB_TILDE, NULL, &glob_result);

    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        if (IsDirectory(glob_result.gl_pathv[i]))
            results.push_back(glob_result.gl_pathv[i]);
    }

    globfree(&glob_result);

    return results;
}

bool WriteFile(const string &path, const string &content)
{
    ofstream file;

    file.open(path, fstream::out | fstream::trunc);

    if (!file.good())
        return false;

    file << content;

    return true;
}

bool RemoveFile(const string &path)
{
    return remove(path.c_str()) == 0;
}

void Daemonize(const string &pidFile)
{
    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        Log(LogCritical) << "fork failed";
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    WriteFile(pidFile, to_string(getpid()));

    on_exit(&DaemonizeExit, const_cast<char*>(pidFile.c_str()));

    umask(0);

    sid = setsid();
    if (sid < 0) {
        Log(LogCritical) << "setsid failed";
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        Log(LogCritical) << "chdir failed";
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void DaemonizeExit(int status, void* arg)
{
    if (arg) {
        char *pidFile = static_cast<char*>(arg);
        Log(LogDebug) << "Removing pidfile '"
            << pidFile << "'";
        RemoveFile(pidFile);
    }
}

};
