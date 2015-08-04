/*
 *   Email : ydzhao1992#gmail.com
 */

#ifndef __APOLLO_TAG_H_H_H__
#define __APOLLO_TAG_H_H_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <utility>

class apollo_tag{
    
    public:
        apollo_tag(const std::string &);
        bool build_tag_value_map();
        //void test_print();
        std::string get_tag_value(const int);
        ~apollo_tag();
    private:
        std::string tag_file_;
        std::map<int,std::string> tag_value_map_;

};



#endif
