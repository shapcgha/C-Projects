#include "searcher.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

void Searcher::add_document(const Filename & filename, std::istream & strm)
{
    if (files.find(filename) != files.end()) {
        remove_document(filename);
    }
    std::string temp;
    size_t count = 0;
    while (strm) {
        strm >> temp;
        if (!isdigit(temp.at(0)) && !isalpha(temp.at(0))) {
            temp.erase(temp.begin());
        }
        if (!isdigit(temp.at(temp.length() - 1)) && !isalpha(temp.at(temp.length() - 1))) {
            temp.erase(--temp.end());
        }
        std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c) { return std::tolower(c); });
        if (table.find(temp) == table.end()) {
            table.insert({temp, std::map<std::string, std::set<size_t>>()});
        }
        if (table.at(temp).find(filename) == table.at(temp).end()) {
            table.at(temp).insert({filename, std::set<size_t>()});
        }
        table.at(temp).at(filename).insert(count++);
    }
    files.insert({filename, std::vector<size_t>()});
}

void Searcher::remove_document(const Filename & filename)
{
    auto it = table.begin();
    while (it != table.end()) {
        if (it->second.find(filename) != it->second.end()) {
            it->second.erase(it->second.find(filename));
        }
        it++;
    }
    files.erase(files.find(filename));
}

using DocIterator = Searcher::DocIterator;
std::pair<DocIterator, DocIterator> Searcher::search(const std::string & query) const
{
    std::map<Filename, std::vector<size_t>> answer = files;
    std::string word;
    std::stringstream words(query);
    bool phrase = false;
    bool phraseNext = false;
    bool notInDocs = false;
    while (words) {
        words >> word;
        if (!isdigit(word.at(0)) && !isalpha(word.at(0))) {
            if (word.at(0) == '\"') {
                phrase = true;
            }
            word.erase(word.begin());
        }
        if (!isdigit(word.at(word.length() - 1)) && !isalpha(word.at(word.length() - 1))) {
            if (word.at(word.length() - 1) == '\"') {
                phraseNext = true;
            }
            word.erase(--word.end());
        }
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        if (table.find(word) == table.end()) {
            notInDocs = true;
            break;
        }
        auto it1 = answer.begin();
        auto it2 = table.find(word)->second.begin();
        while (it1 != answer.end() && it2 != table.find(word)->second.end()) {
            if (it2->first < it1->first) {
                it2++;
            }
            else if (it2->first == it1->first) {
                if (phrase) {
                    if (it1->second.empty()) {
                        std::copy(it2->second.begin(), it2->second.end(), std::back_inserter(it1->second));
                        it1++;
                        it2++;
                    }
                    else {
                        auto itTemp = it1->second.begin();
                        std::vector<size_t> temp;
                        while (itTemp != it1->second.end()) {
                            if (it2->second.find((*itTemp) + 1) != it2->second.end()) {
                                temp.push_back((*itTemp) + 1);
                            }
                            itTemp++;
                        }
                        if (temp.empty()) {
                            answer.erase(it1++);
                            it2++;
                        }
                        else {
                            it1->second = temp;
                            it1++;
                            it2++;
                        }
                    }
                }
                else {
                    it1++;
                    it2++;
                }
            }
            else {
                answer.erase(it1++);
            }
        }
        if (phraseNext) {
            phraseNext = false;
            phrase = false;
        }
        while (it1 != answer.end()) {
            answer.erase(it1++);
        }
    }
    if (notInDocs) {
        return {{},{}};
    }
    std::vector<Filename> temp;
    auto it = answer.begin();
    while (it != answer.end()) {
        temp.push_back(it->first);
        it++;
    }
    auto itVec = answers.insert(temp);
    return {{&*itVec.first->begin()}, {&*itVec.first->end()}};
}