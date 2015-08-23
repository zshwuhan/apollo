/*
 *  Email : ydzhao1992#gmail.com
 */

#ifndef __APOLLO_USER_BOOKMARK_TAG_H_H_H__
#define __APOLLO_USER_BOOKMARK_TAG_H_H_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

#include "tag.h"
#include "bookmark.h"

const std::string RESULT_DIR("../../result/delicious/");
const std::string CLUSTER_DIR("cluster/");
const std::string RECOM_DIR("recom/");
const std::string RESULT_USER_FILE("result_user_cluster");
const std::string RESULT_TAG_FILE("result_tag_cluster");
const std::string RESULT_BOOKMARK_FILE("result_bookmark_cluster");
const std::string ELEM_PER_CLUSTER("50");
const std::string AVE_DEGREE("10");

class apollo_clustering_result{
    
    public:

        apollo_clustering_result(int,int,int,int,int,int);
        apollo_clustering_result();
        ~apollo_clustering_result();

        int u_cnt_;
        int b_cnt_;
        int t_cnt_;

        int u_c_cnt_;
        int b_c_cnt_;
        int t_c_cnt_;

        std::vector<int> u_;
        std::vector<int> b_;
        std::vector<int> t_;

        std::vector<std::set<int> > u_c_;
        std::vector<std::set<int> > b_c_;
        std::vector<std::set<int> > t_c_;

        void clear();

};

class apollo_user_bookmark_tag{
    
    public:

        apollo_user_bookmark_tag(const std::string &,\
                                 const std::string &,\
                                 const std::string &,\
                                 const std::string &,\
                                 int,int,int,int,int);
        ~apollo_user_bookmark_tag();
        bool build_user_bookmark_tag_bigraph();
        void apollo_clustering_(); 
        bool build_bookmark_and_tag_info();
        void output_the_result(const std::string &,\
                               const std::string &,\
                               const std::string &);
        //void test_print();
        void apollo_tripartite_ucf_b(const std::string &);
        void apollo_bipartite_ucf_b(const std::string &);

    private:

        void apollo_recommender_ucf_b(const std::string &);


        void output_the_runtime_status();

        void data_serialization(std::map<int,int>                  &,\
                                std::map<int,int>                  &,\
                                const std::map<int,std::set<int> > &);

        // #0 for user/bookmark/tag
        // #1 for cluster count
        // #2 for element to cluster
        // #3 for cluster to elem

        void set_random_cluster(const std::map<int,int>            &, \
                                int,                                  \
                                std::vector<int>                   &, \
                                std::vector<std::set<int> >        &);
        
        // #0 for which cluster result, such as u_c_,b_c_,t_c_
        // #1 for vector dimension
        // #2 #3 for reverse serialization
        // #4 for edge graph
        // #5 for centroid

        void calculate_cluster_centroid(const std::vector<std::set<int> >  &,\
                                        const int vector_dimension          ,\
                                        const std::map<int,int>            &,\
                                        const std::map<int,int>            &,\
                                        const std::map<int,std::set<int> > &,\
                                        std::vector<std::vector<double> >  & \
                                        );

        // #0 for centroid vector array
        // #1 for norm array
        // #2 for clustering count
        // #3 for dimension count

        void calculate_vector_array_norm(const std::vector<std::vector<double> > &,std::vector<double> &,int,int);

        // #0 #1 for user/bookmark/tag
        // #2 #3 #4 for reverse serialization
        // #5 #6 for centroid vector array
        // #7 #8 for centroid norm
        // #9 #10 for cluster result
        // #11 for cluster count
        void update_new_cluster(const std::map<int,std::set<int> >      &, \
                                const std::map<int,std::set<int> >      &, \
                                const std::map<int,int>                 &, \
                                const std::map<int,int>                 &, \
                                const std::map<int,int>                 &, \
                                const std::vector<std::vector<double> > &, \
                                const std::vector<std::vector<double> > &, \
                                const std::vector<double>               &, \
                                const std::vector<double>               &, \
                                std::vector<int>                        &, \
                                std::vector<std::set<int> >             &, \
                                int);

        // #0 #1 for two type of edge
        // #2 #3 for centroid vector
        // #4 for clustering tmp result

        void user_update_new_cluster(const std::map<int,std::set<int> >      &,\
                                     const std::map<int,std::set<int> >      &,\
                                     const std::vector<std::vector<double> > &,\
                                     const std::vector<std::vector<double> > &,\
                                     apollo_clustering_result                &);

        void bookmark_update_new_cluster(const std::map<int,std::set<int> >     &,\
                                         const std::map<int,std::set<int> >     &,\
                                         const std::vector<std::vector<double> >&,\
                                         const std::vector<std::vector<double> >&,\
                                         apollo_clustering_result               &);

        void tag_update_new_cluster(const std::map<int,std::set<int> >      &,\
                                    const std::map<int,std::set<int> >      &,\
                                    const std::vector<std::vector<double> > &,\
                                    const std::vector<std::vector<double> > &,\
                                    apollo_clustering_result                &);
       

        std::string tag_file_;
        std::string bookmark_file_;
        std::string user_bookmark_tag_file_;
        std::string user_bookmark_tag_test_file_;
        
        apollo_tag tag_;
        apollo_bookmark bookmark_;

        std::map<int,std::set<int> > user_bookmark_;
        std::map<int,std::set<int> > user_tag_;
        std::map<int,std::set<int> > bookmark_user_;
        std::map<int,std::set<int> > bookmark_tag_;
        std::map<int,std::set<int> > tag_user_;
        std::map<int,std::set<int> > tag_bookmark_;

        std::map<int,std::set<int> > user_bookmark_test_;
        std::map<int,std::set<int> > user_tag_test_;
        std::map<int,std::set<int> > bookmark_user_test_;
        std::map<int,std::set<int> > bookmark_tag_test_;
        std::map<int,std::set<int> > tag_user_test_;
        std::map<int,std::set<int> > tag_bookmark_test_;

        std::map<int,int> user_seq_;        // dimension to id
        std::map<int,int> bookmark_seq_;
        std::map<int,int> tag_seq_;

        std::map<int,int> seq_user_;        // id to dimension
        std::map<int,int> seq_bookmark_;
        std::map<int,int> seq_tag_;


        int user_cluster_cnt_;
        int bookmark_cluster_cnt_;
        int tag_cluster_cnt_;

        int user_cnt_;
        int bookmark_cnt_;
        int tag_cnt_;

        int user_test_cnt_;
        int bookmark_test_cnt_;
        int tag_test_cnt_;
        
        apollo_clustering_result cluster_result_; 

        std::time_t start_time;
        std::time_t end_time;
        
        int iter_time_;
        
        std::vector<double> recall_;
        std::vector<double> precision_;
        std::vector<double> fscore_;

        int max_recom_list_length;

};

class apollo_recom_score{
    public:
        apollo_recom_score();
        ~apollo_recom_score();
        friend bool operator<(const apollo_recom_score &,const apollo_recom_score &);
        int id;
        double value;
};

#endif
