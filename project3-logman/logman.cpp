//  PROJECT IDENTIFIER: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
//  logman.cpp
//  project3-logman
//
//  Created by Samyukta Iyengar on 6/2/20.
//  Copyright © 2020 Samyukta Iyengar. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cctype>
#include <set>
#include <iterator>
#include <time.h>

using namespace std;

// log record struct
struct LogRecord {
    LogRecord() { id = 0; }
    LogRecord(size_t id_in) { id = id_in; }
    unsigned long long timestamp;
    string original_timestamp;
    string category;
    string category_lower;
    string message;
    size_t id;
    bool operator < (const LogRecord& l) const {
        return timestamp < l.timestamp;
    }
    bool operator > (const LogRecord& l) const {
        return timestamp >= l.timestamp;
    }
    bool operator () (const LogRecord& l1, const LogRecord& l2) const {
        return l1.timestamp < l2.timestamp;
    }
};

bool compare(LogRecord& l1, LogRecord& l2) {
    if (l1.timestamp == l2.timestamp) {
        if (l1.category_lower == l2.category_lower) {
            return l1.id < l2.id;
        }
        return l1.category_lower < l2.category_lower;
    }
    return l1.timestamp < l2.timestamp;
}

// convert string time to number time
unsigned long long convert_to_time(const string& time_string) {
    unsigned long long t = 0;
    t = (size_t)(stoi(time_string.substr(0, 2)));
    t <<= 8;
    t += (size_t)(stoi(time_string.substr(3, 2)));
    t <<= 8;
    t += (size_t)(stoi(time_string.substr(6, 2)));
    t <<= 8;
    t += (size_t)(stoi(time_string.substr(9, 2)));
    t <<= 8;
    t += (size_t)(stoi(time_string.substr(12, 2)));
    return t;
}

void break_keywords(const string& str, set<string>& v) {
    size_t start = 0;
    size_t end = 0;
    string temp;
    for (size_t i = 0; i < str.size(); i++) {
        if (isalnum(str[i]) == 0) {
            continue;
        }
        start = i;
        while (str[i] != '\0' && (isalnum(str[i]) != 0)) {
            i++;
        }
        end = i;
        temp = str.substr(start, end - start);
        if (!temp.empty()) {
            v.insert(temp);
        }
    }
}


// prints out a log entry in correct format given id, map, and records vector
void print_log_entry(size_t index, const vector<LogRecord>& records) {
    cout << records[index].id << "|" << records[index].original_timestamp << "|" << records[index].category << "|" << records[index].message << "\n";
}

// converts string to all lowercase
void to_lower(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

// trims spaces from a string
void trim_leading_spaces(string& str) {
    size_t start = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] != ' ') {
            start = i;
            break;
        }
    }
    size_t end = str.size() - 1;
    str = str.substr(start, end - start + 1);
}

// prints helpful message for user
void print_helper() {
    cout << "insert helpful message here" << endl;
}

// helper function that reads file info and populates data structures
bool read_file_contents(const string& filename, vector<LogRecord>& records,
                        unordered_map<string, vector<size_t>>& keyword_map,
                        unordered_map<string, vector<size_t>>& category_map,
                        unordered_map<size_t, size_t>& id_to_index) {
    vector<string> file_contents;
    string word;
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        return false;
    }
    size_t counter = 0;
    while (1) {
        word.clear();
        getline(input_file, word, '|');
        if (word.empty()) {
            break;
        }
        LogRecord l;
        l.id = counter++;
        l.original_timestamp = word;
        l.timestamp = convert_to_time(word);
        getline(input_file, word, '|');
        l.category = word;
        getline(input_file, word);
        l.message = word;
        l.category_lower = l.category;
        to_lower(l.category_lower);
        records.push_back(l);
    }
    sort(records.begin(), records.end(), compare);
    cout << counter << " entries read\n";
    for (size_t i = 0; i < records.size(); i++) {
        category_map[records[i].category_lower].push_back(i);
        
        string temp = records[i].message;
        to_lower(temp);
        
        set<string> words;
        break_keywords(temp, words);
        break_keywords(records[i].category_lower, words);
        for (auto s : words) {
            keyword_map[s].push_back(i);
        }
        id_to_index[records[i].id] = i;
    }
    return true;
}

// driver function
int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    if (argc < 2) {
        print_helper();
        return 1;
    }
    string arg = argv[1];
    if (arg == "-h" || arg == "--help") {
        print_helper();
        return 0;
    }
    vector<LogRecord> records;
    unordered_map<string, vector<size_t>> keyword_map;
    unordered_map<string, vector<size_t>> category_map;
    unordered_map<size_t, size_t> id_to_index;
    // reading file contents
    if (!read_file_contents(arg, records, keyword_map, category_map, id_to_index)) {
        cout << "Error: Unable to read file\n";
        exit(0);
    }
    // read standard input
    vector<size_t> search_results;
    deque<size_t> excerpt_list;
    char choice;
    bool searchOccurred = false;
    do {
        cout << "% ";
        cin >> choice;
        switch (choice) {
            case 't':
            {
                string both_timestamps;
                cin >> both_timestamps;
                if (both_timestamps.length() != 29) {
                    break;
                }
                LogRecord time1, time2;
                string temp = both_timestamps.substr(0, 14);
                time1.timestamp = convert_to_time(temp);
                temp = both_timestamps.substr(15, 14);
                time2.timestamp = convert_to_time(temp);
                
                auto lower = lower_bound(records.begin(), records.end(), time1, LogRecord());
                if (lower == records.end()) {
                    cout << "Timestamps search: 0 entries found\n";
                    break;
                }
                auto upper = upper_bound(records.begin(), records.end(), time2, LogRecord());
                search_results.clear();
                while (lower != upper) {
                    search_results.push_back((size_t)(lower - records.begin()));
                    lower++;
                }
                searchOccurred = true;
                cout << "Timestamps search: " << search_results.size() << " entries found\n";
                break;
            }
            case 'm':
            {
                string timestamp_in;
                cin >> timestamp_in;
                if (timestamp_in.length() != 14) {
                    break;
                }
                LogRecord time1;
                time1.timestamp = convert_to_time(timestamp_in);
                auto lower = lower_bound(records.begin(), records.end(), time1, LogRecord());
                if (lower == records.end()) {
                    cout << "Timestamp search: 0 entries found\n";
                    break;
                }
                auto upper = upper_bound(records.begin(), records.end(), time1, LogRecord());
                search_results.clear();
                while (lower != upper) {
                    search_results.push_back(size_t(lower - records.begin()));
                    lower++;
                }
                searchOccurred = true;
                cout << "Timestamp search: " << search_results.size() << " entries found\n";
                break;
            }
            case 'c':
            {
                string category;
                getline(cin, category);
                to_lower(category);
                trim_leading_spaces(category);
                auto it = category_map.find(category);
                search_results.clear();
                if (it != category_map.end()) {
                    const vector<size_t>& v = it->second;
                    for (size_t index : v) {
                        search_results.push_back(index);
                    }
                }
                searchOccurred = true;
                cout << "Category search: " << search_results.size() << " entries found\n";
                break;
            }
            case 'k':
            {
                string keyword;
                getline(cin, keyword);
                to_lower(keyword);
                
                set<string> keywords;
                break_keywords(keyword, keywords);
                if (keywords.size() == 0) {
                    break;
                }
                search_results.clear();
                searchOccurred = true;
                if (keywords.size() == 1) {
                    auto it = keyword_map.find(*(keywords.begin()));
                    if (it == keyword_map.end()) {
                        cout << "Keyword search: 0 entries found\n";
                        break;
                    }
                    search_results = it->second;
                    cout << "Keyword search: " << search_results.size() << " entries found\n";
                    break;
                }
                bool atleastOneKeywordNotFound = false;
                vector<vector<size_t>*> keywords_entry_ids;
                for (auto k : keywords) {
                    auto it = keyword_map.find(k);
                    if (it == keyword_map.end()) {
                        // If one keyword is not found abort the search
                        atleastOneKeywordNotFound = true;
                        break;
                    }
                    keywords_entry_ids.push_back(&it->second);
                }
                if (atleastOneKeywordNotFound) {
                    cout << "Keyword search: 0 entries found\n";
                    break;
                }
                search_results = *keywords_entry_ids[0];
                for (size_t i = 1; i < keywords_entry_ids.size(); i++) {
                    vector<size_t> current_intersection;
                    vector<size_t>& current_entry_id_list = *keywords_entry_ids[i];
                    set_intersection(search_results.begin(),
                                     search_results.end(),
                                     current_entry_id_list.begin(),
                                     current_entry_id_list.end(),
                                     back_inserter(current_intersection));
                    
                    swap(search_results, current_intersection);
                    if (search_results.empty()) {
                        break;
                    }
                }
                cout << "Keyword search: " << search_results.size() << " entries found\n";
                break;
            }
            case 'a':
            {
                size_t entry_id;
                cin >> entry_id;
                if (entry_id >= records.size()) {
                    break;
                }
                excerpt_list.push_back(id_to_index[entry_id]);
                cout << "log entry " << entry_id << " appended\n";
                break;
            }
            case 'r':
                if (!searchOccurred) {
                    break;
                }
                for (size_t i : search_results) {
                    excerpt_list.push_back(i);
                }
                cout << search_results.size() << " log entries appended\n";
                break;
            case 'd':
            {
                size_t index = 0;
                cin >> index;
                
                if (index >= excerpt_list.size()) {
                    break;
                }
                auto it = excerpt_list.begin();
                excerpt_list.erase(it +  (int)index);
                cout << "Deleted excerpt list entry " << index << "\n";
                break;
            }
            case 'b':
            {
                size_t index = 0;
                cin >> index;
                if (index >= excerpt_list.size()) {
                    break;
                }
                if (index == 0) {
                    cout << "Moved excerpt list entry 0\n";
                    break;
                }
                size_t copy = excerpt_list[index];
                auto it = excerpt_list.begin();
                excerpt_list.erase(it + (int)index);
                excerpt_list.push_front(copy);
                cout << "Moved excerpt list entry " << index << "\n";
                break;
            }
            case 'e':
            {
                size_t index = 0;
                cin >> index;
                if (index >= excerpt_list.size()) {
                    break;
                }
                if (index == excerpt_list.size() - 1) {
                    cout << "Moved excerpt list entry " << index << "\n";
                    break;
                }
                size_t copy = excerpt_list[index];
                auto it = excerpt_list.begin();
                excerpt_list.erase(it + (int)index);
                excerpt_list.push_back(copy);
                cout << "Moved excerpt list entry " << index << "\n";
                break;
            }
            case 's':
            {
                cout << "excerpt list sorted\n";
                if (excerpt_list.empty()) {
                    cout << "(previously empty)\n";
                    break;
                }
                cout << "previous ordering:\n";
                cout << "0|";
                print_log_entry(excerpt_list[0], records);
                cout << "...\n";
                cout << excerpt_list.size() - 1 << "|";
                print_log_entry(excerpt_list[excerpt_list.size() - 1], records);
                sort(excerpt_list.begin(), excerpt_list.end());
                cout << "new ordering:\n";
                cout << "0|";
                print_log_entry(excerpt_list[0], records);
                cout << "...\n";
                cout << excerpt_list.size() - 1 << "|";
                print_log_entry(excerpt_list[excerpt_list.size() - 1], records);
                break;
            }
            case 'l':
                cout << "excerpt list cleared\n";
                if (excerpt_list.empty()) {
                    cout << "(previously empty)\n";
                    break;
                }
                cout << "previous contents:\n0|";
                print_log_entry(excerpt_list[0], records);
                cout << "...\n";
                cout << excerpt_list.size() - 1 << "|";
                print_log_entry(excerpt_list[excerpt_list.size() - 1], records);
                excerpt_list.clear();
                break;
            case 'g':
            {
                for (size_t id : search_results) {
                    print_log_entry(id, records);
                }
                break;
            }
            case 'p':
            {
                size_t count = 0;
                for (size_t id : excerpt_list) {
                    cout << count << "|";
                    print_log_entry(id, records);
                    count++;
                }
                break;
            }
            case '#':
                string temp;
                getline(cin, temp);
                break;
        }
    } while (choice != 'q');
}
