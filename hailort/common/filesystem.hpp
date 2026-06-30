/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file filesystem.hpp
 * @brief File system API
 **/

#ifndef _OS_FILESYSTEM_HPP_
#define _OS_FILESYSTEM_HPP_

#include "hailo/hailort.h"
#include "hailo/platform.h"

#include "hailo/expected.hpp"
#include <vector>
#include <string>
#include <chrono>

#include <dirent.h>


namespace hailort
{

class Filesystem final {
public:
    Filesystem() = delete;

    static Expected<std::vector<std::string>> get_files_in_dir_flat(const std::string &dir_path);
    static Expected<std::vector<std::string>> get_latest_files_in_dir_flat(const std::string &dir_path, std::chrono::milliseconds time_interval);
    static Expected<time_t> get_file_modified_time(const std::string &file_path);
    static Expected<bool> is_directory(const std::string &path);
    static hailo_status create_directory(const std::string &dir_path);
    static hailo_status remove_directory(const std::string &dir_path);
    static Expected<std::string> get_current_dir();
    static std::string get_home_directory();
    static bool is_path_accesible(const std::string &path);
    static bool does_file_exists(const std::string &path);

    /**
     * Gets the path to the temporary directory.
     *
     * @return Upon success, returns Expected of the temporary directory path string, ending with /.
     * Otherwise, returns Unexpected of ::hailo_status error.
     */
    static Expected<std::string> get_temp_path();

    static bool has_suffix(const std::string &file_name, const std::string &suffix)
    {
        return (file_name.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), file_name.rbegin());    
    }

    static std::string remove_suffix(const std::string &file_name, const std::string &suffix)
    {
        if (!has_suffix(file_name, suffix)) {
            return file_name;
        }

        return file_name.substr(0, file_name.length() - suffix.length()); 
    }

    // Emultes https://docs.python.org/3/library/os.path.html#os.path.basename
    static std::string basename(const std::string &file_name)
    {
        const auto last_separator_index = file_name.find_last_of(SEPARATOR);
        if (std::string::npos == last_separator_index) {
            // No separator found => the file_name is a "basename"
            return file_name;
        }
        return file_name.substr(last_separator_index + 1); 
    }

private:
    // Filesystem directory separator char.
    static const char *SEPARATOR;

    class DirWalker final {
    public:
        static Expected<DirWalker> create(const std::string &dir_path);
        ~DirWalker();
        DirWalker(const DirWalker &other) = delete;
        DirWalker &operator=(const DirWalker &other) = delete;
        DirWalker &operator=(DirWalker &&other) = delete;
        DirWalker(DirWalker &&other);
        
        dirent* next_file();

    private:
        DirWalker(DIR *dir, const std::string &dir_path);

        DIR *m_dir;
        const std::string m_path_string;
    };
};

class TempFile {
public:
    static Expected<TempFile> create(const std::string &file_name, const std::string &file_directory = "");
    ~TempFile();

    std::string name() const;

private:
    TempFile(const char *path);

    std::string m_path;
};

class LockedFile {
public:
    // The mode param is the string containing the file access mode, compatible with `fopen` function.
    static Expected<LockedFile> create(const std::string &file_path, const std::string &mode);
    ~LockedFile();

    LockedFile(const LockedFile &other) = delete;
    LockedFile &operator=(const LockedFile &other) = delete;
    LockedFile &operator=(LockedFile &&other) = delete;
    LockedFile(LockedFile &&other);

    int get_fd() const;

private:
    LockedFile(FILE *fp, int fd);

    FILE *m_fp;
    int m_fd;
};

} /* namespace hailort */

#endif /* _OS_FILESYSTEM_HPP_ */
