#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

class Searcher
{
public:
    using Filename = std::string; // or std::filesystem::path

    // index modification
    void add_document(const Filename & filename, std::istream & strm);

    void remove_document(const Filename & filename);

    // queries
    class DocIterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = const Filename;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;
        DocIterator(pointer startDoc)
        :nowDoc(startDoc)
        {
        }
        DocIterator()
        :nowDoc(nullptr)
        {
        }
        reference operator*() const { return *nowDoc; }
        pointer operator->() { return nowDoc; }
        DocIterator & operator++(){
            if (nowDoc == nullptr) {
                return *this;
            }
            nowDoc++;
            return *this;
        }
        DocIterator operator++(int)
        {
            DocIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        friend bool operator==(const DocIterator & a, const DocIterator & b) { return a.nowDoc == b.nowDoc; };
        friend bool operator!=(const DocIterator & a, const DocIterator & b) { return a.nowDoc != b.nowDoc; };
    private:
        pointer nowDoc;
    };

    class BadQuery : public std::exception
    {
    };

    std::pair<DocIterator, DocIterator> search(const std::string & query) const;
    std::map<std::string, std::map<Filename, std::set<size_t>>> table;
    std::map<Filename, std::vector<size_t>> files;
    mutable std::set<std::vector<Filename>> answers;
};
