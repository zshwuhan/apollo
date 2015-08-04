/*
 *   Email : ydzhao1992#gmail.com
 */

#include "tag.h"

apollo_tag::apollo_tag(const std::string &tag_file_path):tag_file_(tag_file_path){

}

bool apollo_tag::build_tag_value_map(){
    
    int tag_id;
    std::string tag_id_str;
    std::string tag_name;
    std::ifstream tf(this->tag_file_.c_str());
    
    if(!tf){
        return false;       
    }
    
    while(tf){
        std::getline(tf,tag_id_str);
        std::getline(tf,tag_name);
        std::stringstream tmp;
        tmp << tag_id_str;
        tmp >> tag_id;
        this->tag_value_map_.insert(std::pair<int,std::string>(tag_id,tag_name));
    }

    tf.close();

    return true;

}

std::string apollo_tag::get_tag_value(const int id){
    return this->tag_value_map_.at(id);
}

/*
void apollo_tag::test_print(){
    for(std::map<int,std::string>::iterator iter = this->tag_value_map_.begin() ; iter != this->tag_value_map_.end() ; ++ iter){
        std::cerr << iter->first << "\t" << iter->second << std::endl;
    }
}
*/

apollo_tag::~apollo_tag(){

}
