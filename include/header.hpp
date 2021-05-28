// Copyright 2018 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_
#include "boost/filesystem.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
using boost::filesystem::path;
using boost::filesystem::filesystem_error;
using boost::filesystem::directory_entry;
using boost::filesystem::directory_iterator;
class Broker {
 private:
  std::string Name;
  std::map<std::string, std::pair<int, std::string>>
      statistics;  // <account, <amount of files, last date>>

 public:
  void SetName(const std::string& name) { Name = name; }
  void SetStatistics(const std::string& account, const std::string& date) {
    if (statistics.count(account) == 0) {
      statistics[account].first = 1;
      statistics[account].second = "0";
    } else {
      statistics[account].first++;
    }
    if (statistics[account].second < date) {
      statistics[account].second = date;
    }
  }

  void PrintStatistics() const {
    for (const auto& _x : statistics) {
      std::cout << "broker:" << Name << " account:" << _x.first
                << " files:" << _x.second.first
                << " lastdate:" << _x.second.second << std::endl;
    }
  }
};

class Parser {
 private:
  path path_to_ftp;
  std::map<std::string, Broker> Brokers;

 public:
  explicit Parser(const path& path_to_dir) {
    if (exists(path_to_dir)) {
      if (is_directory(path_to_dir)) {
        path_to_ftp = path_to_dir;
      }
    } else {
      boost::system::error_code code;
      throw(filesystem_error("directory does not exist", code));
    }
  }

  void ParsingDir() {
    for (const directory_entry& obj : directory_iterator{path_to_ftp}) {
      if (is_directory(obj)) {
        std::string broker_name = obj.path().filename().string();
        ReadAndPrintFiles(obj, broker_name);
      } else if (is_symlink(obj)) {
        if (is_directory(read_symlink(obj))) {
          std::string broker_name = obj.path().filename().string();
          ReadAndPrintFiles(obj, broker_name);
        }
      } else if (is_regular_file(obj)) {
        continue;
      }
    }
    if (Brokers.size() != 0) {
      std::cout << std::endl;
      for (const auto& broker : Brokers) {
        broker.second.PrintStatistics();
      }
    }
  }

  void ReadAndPrintFiles(const path& obj, const std::string& broker_name) {
    for (const auto& file : directory_iterator{obj}) {
      auto parsed_filename = ParsingFileName(file);
      if (parsed_filename.first) {
        if (Brokers.count(broker_name) == 0) {
          Brokers.insert({broker_name, {}});
          Brokers[broker_name].SetName(broker_name);
        }
        Brokers[broker_name].SetStatistics(ParseAccount(file),
                                           parsed_filename.second);

        std::cout << broker_name << ' ' << file.path().filename().string()
                  << std::endl;
      }
    }
  }

  std::string ParseAccount(const directory_entry& file) {
    std::string file_str = file.path().filename().string();
    std::string account(file_str.begin() + 8, file_str.begin() + 16);
    return account;
  }

  bool CheckDay(const std::string& month, const std::string& day, bool flag) {
    if (month == "01" || month == "03" || month == "05" || month == "07" ||
        month == "08" || month == "10" || month == "12") {
      if (day >= "01" && day < "32") {
        return true;
      }
    } else if (month == "02") {
      if (flag) {
        if (day >= "01" && day < "30") {
          return true;
        }
      } else {
        if (day >= "01" && day < "29") {
          return true;
        }
      }
    } else {
      if (day >= "01" && day < "31") {
        return true;
      }
    }
    return false;
  }

  std::pair<bool, std::string> ParsingFileName(const directory_entry& file) {
    if (file.path().filename().extension() == ".txt") {
      std::string name = file.path().filename().string();
      std::string part;
      std::istringstream str(name);
      if (name.size() != 29) {
        return {false, ""};
      }
      //std::string temp4(name.begin() + 25, name.begin() + 29);
      //if (temp4 != ".txt") {
      //  return {false, ""};
      //}
      std::getline(str, part, '_');
      if (part == "balance") {
        std::getline(str, part, '_');
        if (part.size() == 8) {
          std::getline(str, part, '.');
          if (part.size() == 8) {
            std::string temp1(part.begin(), part.begin() + 4);
            if (temp1 <= "2020") {
              std::string temp2(part.begin() + 4, part.begin() + 6);
              if ("01" <= temp2 && temp2 < "13") {
                std::string temp3(part.begin() + 6, part.begin() + 8);
                if (std::stoi(temp1) % 4 == 0) {
                  if (CheckDay(temp2, temp3, true)) {
                    std::string last_date = part;
                    return {true, part};
                  }
                } else {
                  if (CheckDay(temp2, temp3, false)) {
                    std::string last_date = part;
                    return {true, part};
                  }
                }
              }
            }
          }
        }
      }
    }
    return {false, ""};
  }
};
#endif // INCLUDE_HEADER_HPP_
