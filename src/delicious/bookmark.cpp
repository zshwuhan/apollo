/*
 *   Email : ydzhao1992#gmail.com
 */

#include "bookmark.h"

apollo_bookmark::apollo_bookmark(const std::string &bookmark_file_path):bookmark_file_(bookmark_file_path){
    
}


bool apollo_bookmark::build_bookmark_value_map(){
    
    int bookmark_id;
    std::string bookmark_id_str;
    std::string bookmark_url;
    std::ifstream tf(this->bookmark_file_.c_str());


    if(!tf){
        return false;
    }

    while(tf){
        std::getline(tf,bookmark_id_str);
        std::getline(tf,bookmark_url);
        std::stringstream tmp;
        tmp << bookmark_id_str ;
        tmp >> bookmark_id;
        //std::cerr << bookmark_id << " " << bookmark_url << std::endl;
        this->bookmark_value_map_.insert(std::pair<int,std::string>(bookmark_id,bookmark_url));
    }

    tf.close();

    return true;

}

/*
void apollo_bookmark::test_print(){
    for(std::map<int,std::string>::iterator iter = this->bookmark_value_map_.begin() ; iter != this->bookmark_value_map_.end() ; ++ iter){
        std::cerr << iter->first << "\t" << iter->second << std::endl;
    }
}
*/

std::string apollo_bookmark::get_bookmark_value(const int id){
    return this->bookmark_value_map_.at(id);
}

apollo_bookmark::~apollo_bookmark(){

}
