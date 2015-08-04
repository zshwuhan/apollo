/*
 *   Email : ydzhao1992#gmail.com
 */

#ifndef __APOLLO_BOOKMARK_H_H_H__
#define __APOLLO_BOOKMARK_H_H_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <utility>

class apollo_bookmark{

    public:
        apollo_bookmark(const std::string &);
        bool build_bookmark_value_map();
        //void test_print();
        std::string get_bookmark_value(const int);
        ~apollo_bookmark();
    private:
        std::string bookmark_file_;
        std::map<int,std::string> bookmark_value_map_;

};

#endif
