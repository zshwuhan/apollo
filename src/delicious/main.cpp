/*
 *  Email : ydzhao1992#gmail.com    
 */

#include "bookmark.h"
#include "tag.h"
#include "user_bookmark_tag.h"

#include <string>

const std::string DATA_DIR("../../data_dst/delicious/");
const std::string TAG_FILE("tags.dat");
const std::string BOOKMARK_FILE("bookmarks.dat");
const std::string USER_BOOKMARK_TAG_TRAIN_FILE("user_bookmark_tag_train.dat");
const std::string USER_BOOKMARK_TAG_TEST_FILE("user_bookmark_tag_test.dat");

const int USER_CLUSTER_CNT = 12;
const int BOOKMARK_CLUSTER_CNT = 46;
const int TAG_CLUSTER_CNT = 14;
const int ITER_TIME = 3;
const int RECOM_LIST = 100;

int main(int argc,char *argv[]){
    
    apollo_user_bookmark_tag u_b_t_(DATA_DIR + TAG_FILE,                      \
                                    DATA_DIR + BOOKMARK_FILE,                 \
                                    DATA_DIR + USER_BOOKMARK_TAG_TRAIN_FILE,  \
                                    DATA_DIR + USER_BOOKMARK_TAG_TEST_FILE,   \
                                    USER_CLUSTER_CNT,                         \
                                    BOOKMARK_CLUSTER_CNT,                     \
                                    TAG_CLUSTER_CNT,                          \
                                    ITER_TIME,                                \
                                    RECOM_LIST);

    u_b_t_.build_user_bookmark_tag_bigraph();
    u_b_t_.build_bookmark_and_tag_info();
    //u_b_t_.apollo_bipartite_ucf_b("apollo_ordinary_bipartite_ucf.txt");
    //u_b_t_.apollo_tripartite_ucf_b("apollo_ordinary_tripartite_ucf_result.txt");
    u_b_t_.apollo_clustering_();
    
    return 0;

}
