#include "searcher.h"

#include <algorithm>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <iterator>

int main(int argc, char ** argv)
{
    Searcher s;
    for (int i = 1; i < argc; ++i) {
        std::ifstream f(argv[i]);
        s.add_document(argv[i], f);
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        const auto [begin, end] = s.search(line);
        std::ostream_iterator<Searcher::Filename> out(std::cout, ", ");
        std::copy(begin, end, out);
        std::cout << std::endl;
    }
    return 0;
}

void print_table(Searcher s)
{
    auto it = s.table.begin();
    while (it != s.table.end()) {
        auto it2 = it->second.begin();
        std::cout << it->first << std::endl;
        while (it2 != it->second.end()) {
            std::cout << it2->first << " ";
            it2++;
        }
        std::cout << std::endl;
        it++;
    }
}