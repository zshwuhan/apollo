/*
 *  Email : ydzhao1992#gmail.com
 */

#include "user_bookmark_tag.h"

apollo_user_bookmark_tag::apollo_user_bookmark_tag(const std::string &tag_file_path,                                    \
                                                   const std::string &bookmark_file_path,                               \
                                                   const std::string &user_bookmark_tag_file_path,                      \
                                                   const std::string &user_bookmark_tag_test_file_path,                 \
                                                   int user_cluster_cnt,int bookmark_cluster_cnt,                       \
                                                   int tag_cluster_cnt,int iter_time,int recom_length):                 \
                                                   tag_file_(tag_file_path),                                            \
                                                   bookmark_file_(bookmark_file_path),                                  \
                                                   user_bookmark_tag_file_(user_bookmark_tag_file_path),                \
                                                   user_bookmark_tag_test_file_(user_bookmark_tag_test_file_path),      \
                                                   tag_(tag_file_path),                                                 \
                                                   bookmark_(bookmark_file_path),                                       \
                                                   user_cluster_cnt_(user_cluster_cnt),                                 \
                                                   bookmark_cluster_cnt_(bookmark_cluster_cnt),                         \
                                                   tag_cluster_cnt_(tag_cluster_cnt),                                   \
                                                   iter_time_(iter_time),max_recom_list_length(recom_length){   
    //
}


bool apollo_user_bookmark_tag::build_user_bookmark_tag_bigraph(){
    
    int user_id;
    int bookmark_id;
    int tag_id;

    std::ifstream tf(this->user_bookmark_tag_file_.c_str());

    if(!tf){
        return false;
    }

    while(tf >> user_id >> bookmark_id >> tag_id ){

        this->user_bookmark_[user_id].insert(bookmark_id);
        this->user_tag_[user_id].insert(tag_id);

        this->bookmark_user_[bookmark_id].insert(user_id);
        this->bookmark_tag_[bookmark_id].insert(tag_id);

        this->tag_user_[tag_id].insert(user_id);
        this->tag_bookmark_[tag_id].insert(bookmark_id);

    }
    
    tf.close();

    tf.open(this->user_bookmark_tag_test_file_.c_str());
    if(!tf){
        return false;
    }

    while(tf >> user_id >> bookmark_id >> tag_id ){

        this->user_bookmark_test_[user_id].insert(bookmark_id);
        this->user_tag_test_[user_id].insert(tag_id);

        this->bookmark_user_test_[bookmark_id].insert(user_id);
        this->bookmark_tag_test_[bookmark_id].insert(tag_id);

        this->tag_user_test_[tag_id].insert(user_id);
        this->tag_bookmark_test_[tag_id].insert(bookmark_id);
    }

    tf.close();

    this->user_cnt_ = this->user_tag_.size();
    this->bookmark_cnt_ = this->bookmark_user_.size();
    this->tag_cnt_ = this->tag_user_.size();

    this->user_test_cnt_ = this->user_tag_test_.size();
    this->bookmark_test_cnt_ = this->bookmark_user_test_.size();
    this->tag_test_cnt_ = this->tag_user_test_.size();

    std::cerr << "train user : " << this->user_cnt_ << std::endl;
    std::cerr << "train bookmark : " << this->bookmark_cnt_ << std::endl;
    std::cerr << "train tag : " << this->tag_cnt_ << std::endl;

    std::cerr << "test user : " << this->user_test_cnt_ << std::endl;
    std::cerr << "test bookmark : " << this->bookmark_test_cnt_ << std::endl;
    std::cerr << "test tag: " << this->tag_test_cnt_ << std::endl;


    std::cerr << "===================================" << std::endl;

    
    this->data_serialization(this->user_seq_,this->seq_user_,this->user_bookmark_);
    this->data_serialization(this->bookmark_seq_,this->seq_bookmark_,this->bookmark_user_);
    this->data_serialization(this->tag_seq_,this->seq_tag_,this->tag_user_);

    return true;
}

void apollo_user_bookmark_tag::data_serialization(std::map<int,int>                  &d_seq_,\
                                                  std::map<int,int>                  &seq_d_,\
                                                  const std::map<int,std::set<int> > &graph){

    int tidx = 0;
    for(std::map<int,std::set<int> >::const_iterator iter = graph.begin() ; iter != graph.end() ; ++ iter){

        d_seq_.insert(std::make_pair(tidx,iter->first));
        seq_d_.insert(std::make_pair(iter->first,tidx));
        ++ tidx;
    
    }
    return ;

}


void apollo_user_bookmark_tag::apollo_clustering_(){
    
    unsigned int t_iter_times = this->iter_time_;
    apollo_clustering_result t_clustering_result(this->user_cnt_,this->bookmark_cnt_,this->tag_cnt_,\
                                                 this->user_cluster_cnt_,this->bookmark_cluster_cnt_,\
                                                 this->tag_cluster_cnt_);
    int tidx;
    int iter_time = 0;
    
    this->start_time = std::time(NULL);
    
    std::vector<std::vector<double> > u_b_(this->user_cluster_cnt_);
    std::vector<std::vector<double> > u_t_(this->user_cluster_cnt_);

//  std::vector<std::vector<double> > b_u_(this->bookmark_cluster_cnt_);
//  std::vector<std::vector<double> > b_t_(this->bookmark_cluster_cnt_);

//  std::vector<std::vector<double> > t_u_(this->tag_cluster_cnt_);
//  std::vector<std::vector<double> > t_b_(this->tag_cluster_cnt_);

    for(tidx = 0 ; tidx < this->user_cluster_cnt_ ; ++ tidx){
        u_b_[tidx].resize(this->bookmark_cnt_,0.0);
        u_t_[tidx].resize(this->tag_cnt_,0.0);
    }

/*
    for(tidx = 0 ; tidx < this->bookmark_cluster_cnt_ ; ++ tidx){
        b_u_[tidx].resize(this->user_cnt_,0.0);
        b_t_[tidx].resize(this->tag_cnt_,0.0);
    }
    
    for(tidx = 0 ; tidx < this->tag_cluster_cnt_ ; ++ tidx){
        t_u_[tidx].resize(this->user_cnt_,0.0);
        t_b_[tidx].resize(this->bookmark_cnt_,0.0);
    }
*/   

    std::srand(std::time(NULL));
    
    this->set_random_cluster(this->user_seq_,this->user_cluster_cnt_,t_clustering_result.u_,t_clustering_result.u_c_);
//  this->set_random_cluster(this->bookmark_seq_,this->bookmark_cluster_cnt_,t_clustering_result.b_,t_clustering_result.b_c_);
//  this->set_random_cluster(this->tag_seq_,this->tag_cluster_cnt_,t_clustering_result.t_,t_clustering_result.t_c_);

    this->cluster_result_ = t_clustering_result;
    
    do{ 

        ++ iter_time;
        
        this->calculate_cluster_centroid(t_clustering_result.u_c_,this->bookmark_cnt_,             \
                                         this->user_seq_,this->bookmark_seq_,this->user_bookmark_, \
                                         u_b_);
        
        this->calculate_cluster_centroid(t_clustering_result.u_c_,this->tag_cnt_,                  \
                                         this->user_seq_,this->tag_seq_,this->user_tag_,           \
                                         u_t_);

//      this->calculate_cluster_centroid(t_clustering_result.b_c_,this->user_cnt_,                 \
                                         this->bookmark_seq_,this->user_seq_,this->bookmark_user_, \
                                         b_u_);
        
//      this->calculate_cluster_centroid(t_clustering_result.b_c_,this->tag_cnt_,                  \
                                         this->bookmark_seq_,this->tag_seq_,this->bookmark_tag_,   \
                                         b_t_);
        
//      this->calculate_cluster_centroid(t_clustering_result.t_c_,this->user_cnt_,                 \
                                         this->tag_seq_,this->user_seq_,this->tag_user_,           \
                                         t_u_);
        
//      this->calculate_cluster_centroid(t_clustering_result.t_c_,this->bookmark_cnt_,             \
                                         this->tag_seq_,this->bookmark_seq_,this->tag_bookmark_,   \
                                         t_b_);
        
        t_clustering_result.clear();
       

        this->user_update_new_cluster(this->user_bookmark_,this->user_tag_,u_b_,u_t_,t_clustering_result);

//      this->bookmark_update_new_cluster(this->bookmark_user_,this->bookmark_tag_,b_u_,b_t_,t_clustering_result);

//      this->tag_update_new_cluster(this->tag_user_,this->tag_bookmark_,t_u_,t_b_,t_clustering_result);


        this->cluster_result_ = t_clustering_result;

        if(iter_time % 1 == 0){

            std::stringstream s;
            std::string iter_time_str;
            s << iter_time;
            s >> iter_time_str;

//          this->output_the_result(\
            RESULT_DIR + CLUSTER_DIR + RESULT_USER_FILE + "_" + AVE_DEGREE + "_" + ELEM_PER_CLUSTER + "_" + iter_time_str,\
            RESULT_DIR + CLUSTER_DIR + RESULT_BOOKMARK_FILE + "_" + AVE_DEGREE + "_" + ELEM_PER_CLUSTER + "_" + iter_time_str,\
            RESULT_DIR + CLUSTER_DIR + RESULT_TAG_FILE + "_" + AVE_DEGREE + "_" + ELEM_PER_CLUSTER + "_" + iter_time_str);
            
            this->apollo_recommender_ucf_b(\
            RESULT_DIR + RECOM_DIR + "ucf_" + AVE_DEGREE + "_" + ELEM_PER_CLUSTER + "_" + iter_time_str);
            
            this->end_time = std::time(NULL);

            std::cerr << "It costs " << this->end_time - this->start_time << " seconds" << std::endl;

            this->start_time = std::time(NULL);


        }
        
        std::cerr << "iter #" << iter_time << std::endl;

    }while(-- t_iter_times);
    

    return ;

}


void apollo_user_bookmark_tag::apollo_bipartite_ucf_b(const std::string &file){

    this->start_time = std::time(NULL);

    this->recall_.resize(this->max_recom_list_length,0.0);
    this->precision_.resize(this->max_recom_list_length,0.0);
    this->fscore_.resize(this->max_recom_list_length,0.0);

    std::ofstream out(file.c_str());

    std::vector<std::vector<int> > mark_b(this->user_cnt_);
    std::vector<double> norm_b(this->user_cnt_,0.0);
    std::vector<std::vector<double> > similar(this->user_cnt_);
    std::vector<std::vector<apollo_recom_score> > score(this->user_cnt_);

    for(size_t iu = 0 ; iu < this->user_cnt_ ; ++ iu){

        mark_b.at(iu).resize(this->bookmark_cnt_,0);
        similar.at(iu).resize(this->user_cnt_,0.0);
        score.at(iu).resize(this->bookmark_cnt_);
        for(size_t ib = 0 ; ib < this->bookmark_cnt_ ; ++ ib){
            score.at(iu).at(ib).id = ib;
            score.at(iu).at(ib).value = 0.0;
        }

    }

    for(std::map<int,std::set<int> >::iterator iter_u = this->user_bookmark_.begin() ; \
                                               iter_u != this->user_bookmark_.end() ;
                                               ++ iter_u){
        int user = iter_u ->first;
        int user_r = this->seq_user_.at(iter_u->first);
        for(std::set<int>::iterator iter_b = iter_u->second.begin() , iter_b_end = iter_u->second.end() ; \
                                    iter_b != iter_b_end ; ++ iter_b){
            int bookmark = *iter_b;
            int bookmark_r = this->seq_bookmark_.at(bookmark);
            mark_b.at(user_r).at(bookmark_r) = 1;
            norm_b.at(user_r) += 1.0;
        }
    }

    for(size_t iu = 0 ; iu < this->user_cnt_ ; ++ iu){
        for(size_t iiu = iu ; iiu < this->user_cnt_ ; ++iiu){
            if(iu == iiu){
                similar.at(iu).at(iiu) = 1.0;
            }else{
                int user = this->user_seq_.at(iu);
                for(std::set<int>::iterator iter_b = this->user_bookmark_.at(user).begin() , \
                                            iter_b_end = this->user_bookmark_.at(user).end() ; \
                                            iter_b != iter_b_end ; ++ iter_b){
                    int bookmark_r = this->seq_bookmark_.at(*iter_b);
                    if(mark_b.at(iiu).at(bookmark_r) == 1){
                        similar.at(iu).at(iiu) += 1.0;
                    }
                }
                similar.at(iu).at(iiu) /= (std::sqrt(norm_b.at(iu)) * std::sqrt(norm_b.at(iiu)));
                similar.at(iiu).at(iu) = similar.at(iu).at(iiu);
            }
        }
    }


    for(size_t iu = 0 ; iu < this->user_cnt_ ; ++ iu){
    
        int user = this->user_seq_.at(iu);

        for(size_t ib = 0 ; ib < this->bookmark_cnt_ ; ++ ib){
            if(mark_b.at(iu).at(ib) == 1){
                score.at(iu).at(ib).value = -1.0;
            }else{
                for(size_t iiu = 0 ; iiu < this->user_cnt_ ; ++ iiu){
                    if(iu == iiu){
                        continue;
                    }else{
                        if(mark_b.at(iiu).at(ib) == 1){
                            score.at(iu).at(ib).value += 1 * similar.at(iu).at(iiu);
                        }
                    }
                }
            }
        }

        std::sort(score.at(iu).begin(),score.at(iu).end());
        std::set<int> tset = this->user_bookmark_test_.at(user);
        std::vector<double> recall(this->max_recom_list_length,0.0);
        std::vector<double> precision(this->max_recom_list_length,0.0);
        
        for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
            
            if(t0 < this->bookmark_cnt_){
                int bookmark = this->bookmark_seq_.at(score.at(iu).at(t0).id);
                if(t0 == 0){
                    if(tset.find(bookmark) != tset.end()){
                        recall.at(t0) = 1.0;
                        precision.at(t0) = 1.0;
                    }else{
                        recall.at(t0) = 0.0;
                        precision.at(t0) = 0.0;
                    }
                }else{
                    if(tset.find(bookmark) != tset.end()){
                        recall.at(t0) = recall.at(t0 - 1) + 1.0;
                        precision.at(t0) = precision.at(t0 - 1) + 1.0;
                    }else{
                        recall.at(t0) = recall.at(t0 - 1);
                        precision.at(t0) = precision.at(t0 - 1);
                    }
                }
            }else{
                if(t0 == 0){
                    recall.at(t0) = 0.0;
                    precision.at(t0) = 0.0;
                }else{
                    recall.at(t0) = recall.at(t0 - 1);
                    precision.at(t0) = precision.at(t0 - 1);
                }
            }  
        
        }
        

        for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
            
            recall.at(t0) /= tset.size();

            if(t0 < this->bookmark_cnt_){
                precision.at(t0) /= (t0 + 1);
            }else{
                precision.at(t0) = precision.at(t0 - 1);
            }

            this->recall_.at(t0) += recall.at(t0);
            this->precision_.at(t0) += precision.at(t0);

        }

    }
    
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
        this->recall_.at(t0) /= static_cast<double>(this->user_cnt_);
        this->precision_.at(t0) /= static_cast<double>(this->user_cnt_);
        if(std::fabs(this->recall_.at(t0)) < 1e-8 && std::fabs(this->precision_.at(t0)) < 1e-8){
            this->fscore_.at(t0) = 0.0;
        }else{
            this->fscore_.at(t0) = (2.0 * this->recall_.at(t0) * this->precision_.at(t0)) / \
                                   (this->recall_.at(t0) + this->precision_.at(t0));
        }
    }

    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->recall_.at(t0) << std::endl;
    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->precision_.at(t0) << std::endl;
    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->fscore_.at(t0) << std::endl;

    this->end_time = std::time(NULL);
    std::cerr << "It costs " << this->end_time - this->start_time << " seconds" << std::endl;

    return ;

}


void apollo_user_bookmark_tag::apollo_tripartite_ucf_b(const std::string &file){


    this->start_time = std::time(NULL);

    this->recall_.resize(this->max_recom_list_length,0.0);
    this->precision_.resize(this->max_recom_list_length,0.0);
    this->fscore_.resize(this->max_recom_list_length,0.0);

    std::ofstream out(file.c_str());

    std::vector<std::vector<int> > mark_b(this->user_cnt_);
    std::vector<std::vector<int> > mark_t(this->user_cnt_);
    std::vector<double> norm_b(this->user_cnt_,0.0);
    std::vector<double> norm_t(this->user_cnt_,0.0);
    std::vector<std::vector<double> > similar(this->user_cnt_);
    std::vector<std::vector<apollo_recom_score> > score(this->user_cnt_);


    for(size_t iu = 0 ; iu != this->user_cnt_ ; ++ iu){
        mark_b.at(iu).resize(this->bookmark_cnt_,0);
        mark_t.at(iu).resize(this->tag_cnt_,0);
        similar.at(iu).resize(this->user_cnt_,0.0);
        score.at(iu).resize(this->bookmark_cnt_);
        for(size_t ib = 0 ; ib != this->bookmark_cnt_ ; ++ ib){
            score.at(iu).at(ib).id = ib;
            score.at(iu).at(ib).value = 0.0;
        }
    }
    
    for(std::map<int,std::set<int> >::iterator iter_u = this->user_bookmark_.begin() ; \
                                               iter_u != this->user_bookmark_.end() ;  \
                                               ++ iter_u){
        int user = iter_u->first;
        int user_r = this->seq_user_.at(user);
        for(std::set<int>::iterator iter_b = iter_u->second.begin() ; iter_b != iter_u->second.end() ; ++ iter_b){
            int bookmark = *iter_b;
            int bookmark_r = this->seq_bookmark_.at(bookmark);
            mark_b.at(user_r).at(bookmark_r) = 1;
            norm_b.at(user_r) += 1.0;
        }
    }

    for(std::map<int,std::set<int> >::iterator iter_u = this->user_tag_.begin() ; \
                                               iter_u != this->user_tag_.end() ; \
                                               ++ iter_u){
        int user = iter_u->first;
        int user_r = this->seq_user_.at(user);
        for(std::set<int>::iterator iter_t = iter_u->second.begin() ; iter_t != iter_u->second.end() ; ++ iter_t){
            int tag = *iter_t;
            int tag_r = this->seq_tag_.at(tag);
            mark_t.at(user_r).at(tag_r) = 1;
            norm_t.at(user_r) += 1.0;
        }
    }

    for(size_t iu = 0 ; iu < this->user_cnt_ ; ++ iu){
        for(size_t iiu = iu ; iiu < this->user_cnt_ ; ++ iiu){
            if(iu == iiu){
                similar.at(iu).at(iiu) = 1.0;
            }else{
                double similar_tmp = 0.0;
                int user = this->user_seq_.at(iu);
                for(std::set<int>::iterator iter_b = this->user_bookmark_.at(user).begin() , \
                                            iter_b_end = this->user_bookmark_.at(user).end() ; \
                                            iter_b != iter_b_end ; \
                                            ++ iter_b){
                    int bookmark = *iter_b;
                    int bookmark_r = this->seq_bookmark_.at(bookmark);
                    if(mark_b.at(iiu).at(bookmark_r) == 1){
                        similar_tmp += 1.0;
                    }
                }
                similar.at(iu).at(iiu) = similar_tmp / (std::sqrt(norm_b.at(iu)) * std::sqrt(norm_b.at(iiu)));
                similar_tmp = 0.0;
                for(std::set<int>::iterator iter_t = this->user_tag_.at(user).begin() , \
                                            iter_t_end = this->user_tag_.at(user).end() ; \
                                            iter_t != iter_t_end ; \
                                            ++ iter_t){
                    int tag = *iter_t;
                    int tag_r = this->seq_tag_.at(tag);
                    if(mark_t.at(iiu).at(tag_r) == 1){
                        similar_tmp += 1.0;
                    } 
                }
                similar.at(iu).at(iiu) = 0.5 * similar.at(iu).at(iiu) + \
                                         0.5 * (similar_tmp / (std::sqrt(norm_t.at(iu)) * std::sqrt(norm_t.at(iiu))));
                similar.at(iiu).at(iu) = similar.at(iu).at(iiu);
            }
        }
    }

    for(size_t iu = 0 ; iu != this->user_cnt_ ; ++ iu){

        int user = this->user_seq_.at(iu);
        for(size_t ib = 0 ; ib != this->bookmark_cnt_ ; ++ ib){
            if(mark_b.at(iu).at(ib) == 1){
                score.at(iu).at(ib).value = -1.0;
            }else{
                for(size_t iiu = 0 ; iiu != this->user_cnt_ ; ++ iiu){
                    if(iu == iiu){
                        continue;
                    }
                    if(mark_b.at(iiu).at(ib) == 1){
                        score.at(iu).at(ib).value += 1 * similar.at(iu).at(iiu);
                    }
                }
            }
        }

        std::sort(score.at(iu).begin(),score.at(iu).end());
        std::set<int> tset = this->user_bookmark_test_.at(user);
    
        std::vector<double> recall(this->max_recom_list_length,0.0);
        std::vector<double> precision(this->max_recom_list_length,0.0);

        for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){

            if(t0 < this->bookmark_cnt_){
                int bookmark = this->bookmark_seq_.at(score.at(iu).at(t0).id);
                if(t0 == 0){
                    if(tset.find(bookmark) != tset.end()){
                        recall.at(t0) = 1.0;
                        precision.at(t0) = 1.0;
                    }else{
                        recall.at(t0) = 0.0;
                        precision.at(t0) = 0.0;
                    }
                }else{
                    if(tset.find(bookmark) != tset.end()){
                        recall.at(t0) = recall.at(t0 - 1) + 1.0;
                        precision.at(t0) = precision.at(t0 - 1) + 1.0;
                    }else{
                        recall.at(t0) = recall.at(t0 - 1);
                        precision.at(t0) = precision.at(t0 - 1);
                    }
                }
            }else{
                if(t0 == 0){
                    recall.at(t0) = 0.0;
                    precision.at(t0) = 0.0;
                }else{
                    recall.at(t0) = recall.at(t0 - 1);
                    precision.at(t0) = precision.at(t0 - 1);
                }
            }  

        }

        for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
            
            recall.at(t0) /= tset.size();

            if(t0 < this->bookmark_cnt_){
                precision.at(t0) /= (t0 + 1);
            }else{
                precision.at(t0) = precision.at(t0 - 1);
            }

            this->recall_.at(t0) += recall.at(t0);
            this->precision_.at(t0) += precision.at(t0);

        }


    }

    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
        this->recall_.at(t0) /= static_cast<double>(this->user_cnt_);
        this->precision_.at(t0) /= static_cast<double>(this->user_cnt_);
        if(std::fabs(this->recall_.at(t0)) < 1e-8 && std::fabs(this->precision_.at(t0)) < 1e-8){
            this->fscore_.at(t0) = 0.0;
        }else{
            this->fscore_.at(t0) = (2.0 * this->recall_.at(t0) * this->precision_.at(t0)) / \
                                   (this->recall_.at(t0) + this->precision_.at(t0));
        }
    }

    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->recall_.at(t0) << std::endl;
    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->precision_.at(t0) << std::endl;
    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->fscore_.at(t0) << std::endl;

    this->end_time = std::time(NULL);
    std::cerr << "It costs " << this->end_time - this->start_time << " seconds" << std::endl;

    return ;
    
}


void apollo_user_bookmark_tag::apollo_recommender_ucf_b(const std::string &file){
    
    size_t cluster_idx = 0;
    
    this->recall_.resize(this->max_recom_list_length,0.0);
    this->precision_.resize(this->max_recom_list_length,0.0);
    this->fscore_.resize(this->max_recom_list_length,0.0);
    
    std::ofstream out(file.c_str());

    for(std::vector<std::set<int> >::iterator iter_c = this->cluster_result_.u_c_.begin();\
                                              iter_c != this->cluster_result_.u_c_.end();\
                                              ++ iter_c){
        ++ cluster_idx;
        

        std::set<int> bookmark_set;
        std::set<int> tag_set;

        std::map<int,int> tmp_user_seq;
        std::map<int,int> tmp_seq_user;
        std::map<int,int> tmp_bookmark_seq;
        std::map<int,int> tmp_seq_bookmark;
        std::map<int,int> tmp_tag_seq;
        std::map<int,int> tmp_seq_tag;

        int uidx = 0;
        int bidx = 0;
        int tidx = 0;
        
        for(std::set<int>::iterator iter_u = iter_c->begin() ; iter_u != iter_c->end() ; ++ iter_u){
            
            int user = this->user_seq_.at(*iter_u);
            tmp_user_seq.insert(std::make_pair(uidx,user));
            tmp_seq_user.insert(std::make_pair(user,uidx));
            ++ uidx;

            for(std::set<int>::iterator iter_b = this->user_bookmark_.at(user).begin(),\
                                    iter_b_end = this->user_bookmark_.at(user).end();\
                                        iter_b != iter_b_end; ++ iter_b){
                if(bookmark_set.find(*iter_b) == bookmark_set.end()){
                    bookmark_set.insert(*iter_b);
                    tmp_bookmark_seq.insert(std::make_pair(bidx,*iter_b));
                    tmp_seq_bookmark.insert(std::make_pair(*iter_b,bidx));
                    ++ bidx;
                }
            }

            for(std::set<int>::iterator iter_t = this->user_tag_.at(user).begin(),\
                                    iter_t_end = this->user_tag_.at(user).end();\
                                        iter_t != iter_t_end ; ++ iter_t){
                if(tag_set.find(*iter_t) == tag_set.end()){
                    tag_set.insert(*iter_t);
                    tmp_tag_seq.insert(std::make_pair(tidx,*iter_t));
                    tmp_seq_tag.insert(std::make_pair(*iter_t,tidx));
                    ++ tidx;
                }
            }
        
        }


        std::vector<std::vector<apollo_recom_score> > score(uidx);

        std::vector<std::vector<int> > mark_b(uidx);
        std::vector<std::vector<int> > mark_t(uidx);

        std::vector<std::vector<double> > similar(uidx);

        std::vector<double> norm_b(uidx,0.0);
        std::vector<double> norm_t(uidx,0.0);

        std::vector<std::vector<double> > recall(uidx);
        std::vector<std::vector<double> > precision(uidx);
        

        for(size_t iu = 0 ; iu != uidx ; ++ iu){
            score.at(iu).resize(bidx);
            for(size_t ib = 0 ; ib != bidx ; ++ ib){
                score.at(iu).at(ib).id = ib;
                score.at(iu).at(ib).value = 0.0;
            }
            mark_b.at(iu).resize(bidx,0);
            mark_t.at(iu).resize(tidx,0);
            similar.at(iu).resize(uidx,0.0);
            recall.at(iu).resize(this->max_recom_list_length,0.0);
            precision.at(iu).resize(this->max_recom_list_length,0.0);
        }


        for(std::set<int>::iterator iter_u = iter_c->begin() ; iter_u != iter_c->end() ; ++ iter_u){

            int user = this->user_seq_.at(*iter_u);  
            int user_r_r = tmp_seq_user.at(user);

            for(std::set<int>::iterator iter_b = this->user_bookmark_.at(user).begin(),\
                                        iter_b_end = this->user_bookmark_.at(user).end();\
                                        iter_b != iter_b_end ; ++ iter_b){
                int bookmark_r_r = tmp_seq_bookmark.at(*iter_b);
                mark_b.at(user_r_r).at(bookmark_r_r) = 1;
                norm_b.at(user_r_r) += 1.0;
            }

            for(std::set<int>::iterator iter_t = this->user_tag_.at(user).begin(),\
                                        iter_t_end = this->user_tag_.at(user).end();\
                                        iter_t != iter_t_end ; ++ iter_t){
                int tag_r_r = tmp_seq_tag.at(*iter_t);
                mark_t.at(user_r_r).at(tag_r_r) = 1;
                norm_t.at(user_r_r) += 1.0;
            }

        }

    
        for(size_t iu = 0 ; iu != uidx ; ++ iu){
            for(size_t iiu = iu ; iiu != uidx ; ++ iiu){

                if(iu == iiu){

                    similar.at(iu).at(iiu) = 1.0;

                }else{

                    double similar_tmp = 0.0;

                    for(size_t ib = 0 ; ib != bidx ; ++ ib){
                        if(mark_b.at(iu).at(ib) == 1 && mark_b.at(iiu).at(ib) == 1){
                            similar_tmp += 1.0;
                        }
                    }
                    
                    similar.at(iu).at(iiu) = similar_tmp / (std::sqrt(norm_b.at(iu)) * std::sqrt(norm_b.at(iiu)));

                    similar_tmp = 0.0;

                    for(size_t it = 0 ; it != tidx ; ++ it){
                        if(mark_t.at(iu).at(it) == 1 && mark_t.at(iiu).at(it) == 1){
                            similar_tmp += 1.0;
                        }
                    }

                    similar.at(iu).at(iiu) = 0.5 * similar.at(iu).at(iiu) + \
                                             0.5 * similar_tmp / (std::sqrt(norm_t.at(iu)) * std::sqrt(norm_t.at(iiu)));

                    similar.at(iiu).at(iu) = similar.at(iu).at(iiu);

                }
            }
        }
        

        for(size_t iu = 0 ; iu != uidx ; ++ iu){

            int user_r_r = tmp_user_seq.at(iu);
            std::set<int> tset = this->user_bookmark_test_.at(user_r_r);
            
            for(size_t ib = 0; ib != bidx ; ++ ib){
                for(size_t iiu = 0 ; iiu != uidx ; ++ iiu){
                    if(mark_b.at(iu).at(ib) == 1){
                        score.at(iu).at(ib).value = -1.0;
                    }else{
                        if(iiu == iu){
                            continue;
                        }else{
                            if(mark_b.at(iiu).at(ib) == 1){
                                score.at(iu).at(ib).value += 1 * similar.at(iu).at(iiu);
                            }
                        }
                    }
                }
            }

            std::sort(score.at(iu).begin() , score.at(iu).end());
            
            for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
                if(t0 < bidx){
                    int bookmark_r_r = tmp_bookmark_seq.at(score.at(iu).at(t0).id);
                    if(t0 == 0){
                        if(tset.find(bookmark_r_r) != tset.end()){
                            recall.at(iu).at(t0) = 1.0;
                            precision.at(iu).at(t0) = 1.0;
                        }else{
                            recall.at(iu).at(t0) = 0.0;
                            precision.at(iu).at(t0) = 0.0;
                        }
                    }else{
                        if(tset.find(bookmark_r_r) != tset.end()){
                            recall.at(iu).at(t0) =  recall.at(iu).at(t0-1) + 1.0;
                            precision.at(iu).at(t0) = precision.at(iu).at(t0-1) + 1.0;
                        }else{
                            recall.at(iu).at(t0) = recall.at(iu).at(t0-1);
                            precision.at(iu).at(t0) = precision.at(iu).at(t0-1);
                        }
                    }
                }else{
                    if(t0 == 0){
                        recall.at(iu).at(t0) = 0.0;
                        precision.at(iu).at(t0) = 0.0;
                    }else{
                        recall.at(iu).at(t0) = recall.at(iu).at(t0-1);
                        precision.at(iu).at(t0) = precision.at(iu).at(t0-1);
                    }
                }
            }
            

            for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
                
                recall.at(iu).at(t0) /= tset.size();

                if(t0 < bidx){
                    precision.at(iu).at(t0) /= (t0 + 1);
                }else{
                    precision.at(iu).at(t0) = precision.at(iu).at(t0-1);
                }

                this->recall_.at(t0) += recall.at(iu).at(t0);
                this->precision_.at(t0) += precision.at(iu).at(t0);

            }

        }
        
    
    }

    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0){
        this->recall_.at(t0) /= static_cast<double>(this->user_cnt_);
        this->precision_.at(t0) /= static_cast<double>(this->user_cnt_);
        if(std::fabs(this->recall_.at(t0)) < 1e-8 && std::fabs(this->precision_.at(t0)) < 1e-8){
            this->fscore_.at(t0) = 0;
        }else{
            this->fscore_.at(t0) = (2.0 * this->recall_.at(t0) * this->precision_.at(t0)) / \
                                   (this->recall_.at(t0) + this->precision_.at(t0));
        }

    }
    

    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->recall_.at(t0) << std::endl;
    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->precision_.at(t0) << std::endl;
    out << std::endl << std::endl << std::endl;
    for(size_t t0 = 0 ; t0 < this->max_recom_list_length ; ++ t0)
        out << this->fscore_.at(t0) << std::endl;


    return ;

}

void apollo_user_bookmark_tag::set_random_cluster(const std::map<int,int> &d_seq ,          \
                                                  int max_cluster_cnt,                      \
                                                  std::vector<int> &elem_cluster,           \
                                                  std::vector<std::set<int> > &cluster_elem){
    int random_start = rand() % max_cluster_cnt;
    for(std::map<int,int>::const_iterator iter = d_seq.begin() ; iter != d_seq.end() ; ++ iter){
        elem_cluster.at(iter->first) = random_start;
        cluster_elem.at(random_start).insert(iter->first);
        random_start = (random_start + 1) % max_cluster_cnt;
    }

    return ;

}

void apollo_user_bookmark_tag::calculate_cluster_centroid(const std::vector<std::set<int> >  &cluster_c             ,\
                                                          const int vector_dimension                                ,\
                                                          const std::map<int,int> &c_seq                            ,\
                                                          const std::map<int,int> &d_seq                            ,\
                                                          const std::map<int,std::set<int> > &graph                 ,\
                                                          std::vector<std::vector<double> >  &centroid               \
                                                          ){

    int cidx = 0;

    for(std::vector<std::set<int> >::const_iterator iter_c = cluster_c.begin() ; \
                                                    iter_c != cluster_c.end()  ; \
                                                    ++ iter_c){
        
        for(int didx = 0 ; didx != vector_dimension ; ++ didx){

            unsigned int edgecnt = 0;

            for(std::set<int>::const_iterator iter_c_cluster = iter_c->begin() ; \
                                              iter_c_cluster != iter_c->end()  ; \
                                              ++ iter_c_cluster){

                int c_elem_rseq = c_seq.at(*iter_c_cluster);

                int d_elem_rseq = d_seq.at(didx);
                    
                if(graph.at(c_elem_rseq).find(d_elem_rseq) != graph.at(c_elem_rseq).end()){
                    ++ edgecnt;
                }
                
            }
            
            centroid.at(cidx).at(didx) = static_cast<double>(edgecnt) / static_cast<double>(iter_c->size());
        
        }

        cidx ++;
    
    }

    return ;

}

void apollo_user_bookmark_tag::update_new_cluster(const std::map<int,std::set<int> >        &x,\
                                                  const std::map<int,std::set<int> >        &y,\
                                                  const std::map<int,int>                   &seqx,\
                                                  const std::map<int,int>                   &seqy,\
                                                  const std::map<int,int>                   &seqz,\
                                                  const std::vector<std::vector<double> >   &centroidx,\
                                                  const std::vector<std::vector<double> >   &centroidy,\
                                                  const std::vector<double>                 &centroidxnorm,\
                                                  const std::vector<double>                 &centroidynorm,\
                                                  std::vector<int>                          &clusterres,\
                                                  std::vector<std::set<int> >               &clusterresgroup,\
                                                  int cluster_cnt){

    const double alpha = 0.5;

    for(std::map<int,std::set<int> >::const_iterator iterout = x.begin() ; \
                                                     iterout != x.end()  ; \
                                                     ++ iterout){
        
        std::vector<double> similar(cluster_cnt,0.0);
        int xid = iterout->first;
        double tmulval;
        double tsqrtval;

        for(int tclusteridx = 0 ; tclusteridx != cluster_cnt ; ++ tclusteridx){

            tmulval = 0.0;
            tsqrtval = std::sqrt(x.at(xid).size());
            
            for(std::set<int>::iterator iterx = x.at(xid).begin() , iterxend = x.at(xid).end();\
                                        iterx != iterxend         ;\
                                        ++ iterx){
                
                
                int seqxid = seqx.at(*iterx);
                tmulval += centroidx.at(tclusteridx).at(seqxid);
            
            }

            similar.at(tclusteridx) = alpha * (tmulval / (tsqrtval * centroidxnorm.at(tclusteridx)));

            tmulval = 0.0;
            tsqrtval = std::sqrt(y.at(xid).size());

            for(std::set<int>::iterator itery = y.at(xid).begin() , iteryend = y.at(xid).end() ; \
                                        itery != iteryend         ; \
                                        ++ itery){
            
                int seqyid = seqy.at(*itery);
                tmulval += centroidy.at(tclusteridx).at(seqyid);

            }

            similar.at(tclusteridx) += (1-alpha) * (tmulval / (tsqrtval * centroidynorm.at(tclusteridx)));
        
        
        }

        double maxval = similar.at(0);
        int loc = 0;

        for(int tclusteridx = 1 ; tclusteridx != cluster_cnt ; ++ tclusteridx){
            if(maxval - similar.at(tclusteridx) < 1e-8){
                maxval = similar.at(tclusteridx);
                loc = tclusteridx;
            }
        }

        int zid = seqz.at(xid);
        
        clusterres.at(zid) = loc;
        clusterresgroup.at(loc).insert(zid);


    }


    return ;

}

void apollo_user_bookmark_tag::user_update_new_cluster(const std::map<int,std::set<int> >      &x,\
                                                       const std::map<int,std::set<int> >      &y,\
                                                       const std::vector<std::vector<double> > &u_b_,\
                                                       const std::vector<std::vector<double> > &u_t_,\
                                                       apollo_clustering_result                &res){
    
    std::vector<double> centroid_u_b_norm(this->user_cluster_cnt_,0.0);
    this->calculate_vector_array_norm(u_b_,centroid_u_b_norm,this->user_cluster_cnt_,this->bookmark_cnt_);

    std::vector<double> centroid_u_t_norm(this->user_cluster_cnt_,0.0);
    this->calculate_vector_array_norm(u_t_,centroid_u_t_norm,this->user_cluster_cnt_,this->tag_cnt_);

    this->update_new_cluster(x,y,this->seq_bookmark_,this->seq_tag_,this->seq_user_,   \
                             u_b_,u_t_,centroid_u_b_norm,centroid_u_t_norm,            \
                             res.u_,res.u_c_,this->user_cluster_cnt_);

    return ;

}

void apollo_user_bookmark_tag::bookmark_update_new_cluster(const std::map<int,std::set<int> >      &x,\
                                                           const std::map<int,std::set<int> >      &y,\
                                                           const std::vector<std::vector<double> > &b_u_ ,\
                                                           const std::vector<std::vector<double> > &b_t_ ,\
                                                           apollo_clustering_result                &res){
    
    std::vector<double> centroid_b_u_norm(this->bookmark_cluster_cnt_,0.0);
    this->calculate_vector_array_norm(b_u_,centroid_b_u_norm,this->bookmark_cluster_cnt_,this->user_cnt_);

    std::vector<double> centroid_b_t_norm(this->bookmark_cluster_cnt_,0.0);
    this->calculate_vector_array_norm(b_t_,centroid_b_t_norm,this->bookmark_cluster_cnt_,this->tag_cnt_);

    this->update_new_cluster(x,y,this->seq_user_,this->seq_tag_,this->seq_bookmark_,    \
                             b_u_,b_t_,centroid_b_u_norm,centroid_b_t_norm,             \
                             res.b_,res.b_c_,this->bookmark_cluster_cnt_);

    return ;

}


void apollo_user_bookmark_tag::tag_update_new_cluster(const std::map<int,std::set<int> >      &x,\
                                                      const std::map<int,std::set<int> >      &y,\
                                                      const std::vector<std::vector<double> > &t_u_,\
                                                      const std::vector<std::vector<double> > &t_b_,\
                                                      apollo_clustering_result                &res){
    
    std::vector<double> centroid_t_u_norm(this->tag_cluster_cnt_,0.0);
    this->calculate_vector_array_norm(t_u_,centroid_t_u_norm,this->tag_cluster_cnt_,this->user_cnt_);

    std::vector<double> centroid_t_b_norm(this->tag_cluster_cnt_,0.0);
    this->calculate_vector_array_norm(t_b_,centroid_t_b_norm,this->tag_cluster_cnt_,this->bookmark_cnt_);

    this->update_new_cluster(x,y,this->seq_user_,this->seq_bookmark_,this->seq_tag_,    \
                             t_u_,t_b_,centroid_t_u_norm,centroid_t_b_norm,             \
                             res.t_,res.t_c_,this->tag_cluster_cnt_);

    return ;

}


void apollo_user_bookmark_tag::calculate_vector_array_norm(const std::vector<std::vector<double> > &c,\
                                                           std::vector<double> &r,\
                                                           int outer,\
                                                           int inner){
    
    for(int iter_out = 0 ; iter_out != outer ; ++ iter_out){
        for(int iter_in = 0 ; iter_in != inner ; ++ iter_in){
            r.at(iter_out) += std::pow(c.at(iter_out).at(iter_in),2.0);
        }
        r.at(iter_out) = std::sqrt(r.at(iter_out));
    }
    
    return ;

}

bool apollo_user_bookmark_tag::build_bookmark_and_tag_info(){
    
    if(!this->tag_.build_tag_value_map()){
        return false;
    }

    if(!this->bookmark_.build_bookmark_value_map()){
        return false;
    }

    return true;

}

void apollo_user_bookmark_tag::output_the_result(const std::string &output_user_file,    \
                                                 const std::string &output_bookmark_file,\
                                                 const std::string &output_tag_file){
    
    std::ofstream tf(output_user_file.c_str());
    for(int idx = 0 ; idx < this->user_cluster_cnt_ ; ++ idx){

        tf << idx << " " << this->cluster_result_.u_c_.at(idx).size() << std::endl;
    
        std::set<int>::iterator iterend = this->cluster_result_.u_c_.at(idx).end();

        for(std::set<int>::iterator iter = this->cluster_result_.u_c_.at(idx).begin() ;\
                                    iter != iterend                                   ;\
                                    ++ iter){
            
            int user_rseq = this->user_seq_.at(*iter);
            tf << user_rseq << std::endl;

        }
    
    }
    
    tf.close();

    tf.open(output_bookmark_file.c_str());
    for(int idx = 0 ; idx < this->bookmark_cluster_cnt_ ; ++ idx){
        
        tf << idx << " " << this->cluster_result_.b_c_.at(idx).size() << std::endl;
        
        std::set<int>::iterator iterend = this->cluster_result_.b_c_.at(idx).end();

        for(std::set<int>::iterator iter = this->cluster_result_.b_c_.at(idx).begin() ; \
                                    iter != iterend                                   ; \
                                    ++ iter){
            
            int bookmark_rseq = this->bookmark_seq_.at(*iter);
            tf << bookmark_rseq << " " << this->bookmark_.get_bookmark_value(bookmark_rseq) << std::endl;
        
        }

    }

    tf.close();

    tf.open(output_tag_file.c_str());

    for(int idx = 0 ; idx < this->tag_cluster_cnt_ ; ++ idx){
        
        tf << idx << " " << this->cluster_result_.t_c_.at(idx).size() << std::endl;

        std::set<int>::iterator iterend = this->cluster_result_.t_c_.at(idx).end();

        for(std::set<int>::iterator iter = this->cluster_result_.t_c_.at(idx).begin() ; \
                                    iter != iterend                                   ; \
                                    ++ iter){
            
            int tag_rseq = this->tag_seq_.at(*iter);
            tf << tag_rseq << " " << this->tag_.get_tag_value(tag_rseq) << std::endl;

        }

    }

    return ;

}
/*
void apollo_user_bookmark_tag::test_print(){
    
    this->bookmark_.test_print();
    this->tag_.test_print();

}
*/

void apollo_user_bookmark_tag::output_the_runtime_status(){
    

    std::cerr << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    
    for(int idx = 0 ; idx < this->user_cluster_cnt_ ; ++ idx){
        std::cerr << "user cluster #" << idx << " : " << this->cluster_result_.u_c_.at(idx).size() << std::endl;
    }
    for(int idx = 0 ; idx < this->bookmark_cluster_cnt_ ; ++ idx){
        std::cerr << "bookmark cluster #" << idx << " : " << this->cluster_result_.b_c_.at(idx).size() << std::endl;
    }
    for(int idx = 0 ; idx < this->tag_cluster_cnt_ ; ++ idx){
        std::cerr << "tag cluster #" << idx << " : " << this->cluster_result_.t_c_.at(idx).size() << std::endl;
    }

    std::cerr << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    
    return ;

}

apollo_user_bookmark_tag::~apollo_user_bookmark_tag(){
    //
}

apollo_clustering_result::apollo_clustering_result(){
    //
}

apollo_clustering_result::apollo_clustering_result(int u_cnt,int b_cnt,int t_cnt,\
                                                   int u_c_cnt,int b_c_cnt,int t_c_cnt):\
                                                   u_cnt_(u_cnt),b_cnt_(b_cnt),t_cnt_(t_cnt),\
                                                   u_c_cnt_(u_c_cnt),b_c_cnt_(b_c_cnt),t_c_cnt_(t_c_cnt),\
                                                   u_(u_cnt_),b_(b_cnt_),t_(t_cnt),u_c_(u_c_cnt_),\
                                                   b_c_(b_c_cnt_),t_c_(t_c_cnt_){
    //
}

void apollo_clustering_result::clear(){
    

    for(size_t idx = 0 ; idx != this->u_c_cnt_ ; ++ idx){
        this->u_c_.at(idx).clear();
    }

    for(size_t idx = 0 ; idx != this->b_c_cnt_ ; ++ idx){
        this->b_c_.at(idx).clear();
    }

    for(size_t idx = 0 ; idx != this->t_c_cnt_ ; ++ idx){
        this->t_c_.at(idx).clear();
    }

    return ;

}

apollo_clustering_result::~apollo_clustering_result(){
    //
}

apollo_recom_score::apollo_recom_score():id(0),value(0.0){
    //
}

apollo_recom_score::~apollo_recom_score(){
    //
}

bool operator<(const apollo_recom_score &a,const apollo_recom_score &b){
    return a.value < b.value;
}

