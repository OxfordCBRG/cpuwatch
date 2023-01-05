#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

const int SAMPLE_RATE_SECONDS = 5;
const int MAX_SYSTEM_UID = 999; // Ignore all processes owned by UIDs <= MAX_SYSTEM_UID
const float SCALING_FACTOR = 2; // 100% CPU * SCALING_FACTOR = 1 niceness unit

// Read the whole of a file into a single string, removing the final '\n'
inline const string file_to_string(const string &path)
{
    ifstream ifs(path);
    const string contents((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
    return(contents.substr(0,contents.length()-1));
}
 
// Get a file as a vector of strings, splits on any character in [:space:]
inline const vector<string> split_on_space(const string &path)
{
    istringstream iss(path);
    return vector<string>((istream_iterator<string>(iss)),istream_iterator<string>());
}

inline void do_nice(uid_t uid, int newnice)
{
    errno = 0;
    int oldnice = getpriority(PRIO_USER, uid);
    try
    {
        if (oldnice == -1 && errno)
            return; // User may have logged out, nothing we can do
        if (oldnice == newnice)
            return; // Pointless
        if (oldnice == 19)
            return; // Reserved for sysadmins
        if (setpriority(PRIO_USER, uid, newnice) < 0)
            throw runtime_error("Setpriority failed");
    }
    catch(const exception& e)
    {
        cerr << "WARN: " << e.what() << '\n'; // Don't crash, just log it.
    }
}

inline void calc_nice(unordered_map<uid_t, float> &past, unordered_map<uid_t, float> &present)
{
    for (const auto & [uid, cpu] : present)
    {
        if (past.contains(uid))
        {
            float delta = present[uid] - past[uid];
            float cpus_used = delta / 100 / SAMPLE_RATE_SECONDS;
            int nice = (int)(cpus_used * SCALING_FACTOR);
            if (nice >= 18)
                do_nice(uid, 18);
            else if (nice >= 0)
                do_nice(uid, nice);
            // Ignore everything < 0
        }
    }
}

inline void get_data(unordered_map<uid_t, float> &present)
{
    for (const auto & pd : filesystem::directory_iterator("/proc/"))
    {
        try
        {
            const auto full_path = pd.path();
            const string pid = full_path.filename();
            if (! all_of(pid.begin(), pid.end(), ::isdigit)) // Not a PID
                continue;
            const string pid_root = "/proc/" + pid;
            struct stat statbuf;
            if (lstat(pid_root.c_str(), &statbuf) == -1)
                continue; // Process probably exited. Don't care.
            if (statbuf.st_uid <= MAX_SYSTEM_UID)
                continue; // Ignore system processes
            present.try_emplace(statbuf.st_uid, 0.0); // Does nothing if key exists
            const auto stats = split_on_space(file_to_string(pid_root + "/stat"));
            // Total CPU time is the sum of user time and system time
            const float cputime = stof(stats.at(13)) + stof(stats.at(14));
            present[statbuf.st_uid] += cputime;
        }
        catch (const exception &e)
        {
            cerr << "WARN: " << e.what() << '\n'; // Don't crash, just log it.
        }
    }
}

int main()
{
    unordered_map<uid_t, float> past, present;
    get_data(past);
    while (true)
    {
        sleep(SAMPLE_RATE_SECONDS);
        get_data(present);
        calc_nice(past, present);
        past = present;
        present.clear();
    }
    return 0;
}