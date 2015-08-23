#! /bin/python2
# -*- coding : utf-8 -*-

import string
import os
import inspect

class obj:
    def __init__(self):
        self.obj_cnt_map_ = {}

    def update_cnt(self,xid):
        if self.obj_cnt_map_.has_key(xid):
            self.obj_cnt_map_[xid] += 1
        else:
            self.obj_cnt_map_[xid] = 1

    def filter_degree(self,cnt):
        tmp_obj_cnt_map_ = {u:self.obj_cnt_map_[u] for u in self.obj_cnt_map_.keys() if self.obj_cnt_map_[u] > cnt}
        self.obj_cnt_map_ = tmp_obj_cnt_map_

    def check_id(self,xid):
        if self.obj_cnt_map_.has_key(xid):
            return True
        else:
            return False


class user(obj):

    def __init__(self):
        obj.__init__(self)


class tags(obj):

    def __init__(self):
        obj.__init__(self)
        self.tags_value_map_ = {}
        self.tags_cnt_map_ = {}

    def build_tags_value_map(self,tag_path):
        
        try:

            with open(tag_path,"r") as tf:

                #filter the first line

                tl = tf.readline();
                tl = tf.readline();

                while tl:
                    
                    tl = tl.strip()

                    [ttagid,ttagname] = tl.split("\t")
             
                    try:
                        ttagid = int(ttagid)
                    except Exception,e:
                        print str(e)
                        tl = tf.readline()
                        continue
                    
                    self.tags_value_map_[ttagid] = ttagname

                    tl = tf.readline()


        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename)+":"+str(inspect.currentframe().f_lineno)
            exit(-1)



    def check_tags_id(self,tag_id):

        if self.tags_value_map_.has_key(tag_id):
            return True
        else:
            return False


class bookmarks(obj):

    def __init__(self):
        obj.__init__(self)
        self.bookmarks_value_map_ = {}
        self.bookmarks_cnt_map_ = {}

    
    def build_bookmarks_value_map(self,bookmark_path):

        try:

            with open(bookmark_path,"r") as tf:
                
                # filter the first line
                tl = tf.readline()
                tl = tf.readline()

                while tl:

                    tl = tl.strip()
                    [tbid,tmd5title,ttitle,turl,tmd5prin,turlprin] = tl.split("\t")
                    
                    try:
                        tbid = int(tbid)
                    except Exception,e:
                        print str(e)
                        tl = tl.readline()
                        continue

                    self.bookmarks_value_map_[tbid] = turl
                    tl = tf.readline()

        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)
    


    def check_bookmarks_id(self,bookmark_id):

        if self.bookmarks_value_map_.has_key(bookmark_id):
            return True
        else:
            return False



class data_wash:
    
    def __init__(self,data_src_dir,data_dst_dir,user_bookmark_tag_file,tag_file,bookmark_file,readme_file):

        self.user_ = user()
        self.tags_ = tags()
        self.bookmarks_ = bookmarks()
        self.data_src_dir_ = data_src_dir
        self.data_dst_dir_ = data_dst_dir
        self.u_b_t_file_ = user_bookmark_tag_file
        self.o_u_b_t_file_ = "user_bookmark_tag_train.dat"
        self.o_u_b_t_test_file_ = "user_bookmark_tag_test.dat"
        self.t_file_ = tag_file
        self.b_file_ = bookmark_file
        self.u_b_t_list_ = []
        self.readme_file_ = readme_file
        self.degree_filter_param_ = 10

        self.train_user_set_ = set()
        self.train_bookmark_set_ = set()
        self.train_tag_set_ = set()

        self.test_user_set_ = set()
        self.test_bookmark_set_ = set()
        self.test_tag_set_ = set()

        self.train_u_b_t_cnt_ = 0
        self.test_u_b_t_cnt_ = 0

    def handler_data(self):
        
        self.tags_.build_tags_value_map(self.data_src_dir_ + "/" + self.t_file_)
        self.bookmarks_.build_bookmarks_value_map(self.data_src_dir_ + "/" + self.b_file_)

        try:
            with open(self.data_src_dir_+"/"+self.u_b_t_file_,"r") as tf:
            
                # filter first line
                tl = tf.readline()

                tl = tf.readline()
            
                while tl:
                
                    tl = tl.strip()
                    [tuser,tbookmark,ttag,ttime] = tl.split('\t')

                    try:
                        tuser = int(tuser)
                        tbookmark = int(tbookmark)
                        ttag = int(ttag)
                        ttime = int(ttime)
                    except Exception,te:
                        print str(te)
                        tl = tf.readline()
                        continue

                    if self.bookmarks_.check_bookmarks_id(tbookmark) and self.tags_.check_tags_id(ttag):
                        self.user_.update_cnt(tuser)
                        self.bookmarks_.update_cnt(tbookmark)
                        self.tags_.update_cnt(ttag)
                        self.u_b_t_list_.append((tuser,ttime,tbookmark,ttag))
                    
                    tl = tf.readline()

        except Exception,e:
            print str(e)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)
        
        while True:

            old_user_cnt = len(self.user_.obj_cnt_map_)
            old_bookmarks_cnt = len(self.bookmarks_.obj_cnt_map_)
            old_tags_cnt = len(self.tags_.obj_cnt_map_)

            self.user_.filter_degree(self.degree_filter_param_)
            self.bookmarks_.filter_degree(self.degree_filter_param_)
            self.tags_.filter_degree(self.degree_filter_param_)

            tmp_u_b_t_list = [u for u in self.u_b_t_list_ if self.user_.check_id(u[0]) \
                                                         and self.bookmarks_.check_id(u[2]) \
                                                         and self.tags_.check_id(u[3])]
            
            self.user_.obj_cnt_map_ = {}
            self.bookmarks_.obj_cnt_map_ = {}
            self.tags_.obj_cnt_map_ = {}
            
            self.u_b_t_list_ = tmp_u_b_t_list

            for item in self.u_b_t_list_:
                self.user_.update_cnt(item[0])
                self.bookmarks_.update_cnt(item[2])
                self.tags_.update_cnt(item[3])
            

            if old_user_cnt == len(self.user_.obj_cnt_map_) and \
               old_bookmarks_cnt == len(self.bookmarks_.obj_cnt_map_) and \
               old_tags_cnt == len(self.tags_.obj_cnt_map_) :
                   break

        
        tmp_bookmarks_value_map = {u:self.bookmarks_.bookmarks_value_map_[u] \
                                   for u in self.bookmarks_.bookmarks_value_map_.keys() if \
                                   self.bookmarks_.check_id(u)}
        self.bookmarks_.bookmarks_value_map_ = tmp_bookmarks_value_map

        tmp_tags_value_map = {u:self.tags_.tags_value_map_[u] \
                              for u in self.tags_.tags_value_map_.keys() if self.tags_.check_id(u)}
        self.tags_.tags_value_map_ = tmp_tags_value_map

        self.u_b_t_list_ = sorted(self.u_b_t_list_)

    
    def generate_data(self):
        
        try:
            if os.path.exists(self.data_dst_dir_):
                print self.data_dst_dir_ + " exists !"
            else:
                print self.data_dst_dir_ + " does not exists , creating !"
                os.makedirs(self.data_dst_dir_)

        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)
        
        try:
            with open(self.data_dst_dir_ + "/" + self.o_u_b_t_file_ , "w") as tf:
                try:
                    with open(self.data_dst_dir_ + "/" + self.o_u_b_t_test_file_ , "w") as tft:

                        current_user = -1
                        tmp_four_set = []

                        for idx in range(0,len(self.u_b_t_list_)):
                            if(self.u_b_t_list_[idx][0] != current_user):

                                current_user = self.u_b_t_list_[idx][0]
                                tmp_four_set_len = len(tmp_four_set)

                                if tmp_four_set_len != 0 :

                                    if tmp_four_set_len > self.degree_filter_param_:

                                        tmp_four_train_set_len = int(0.8 * tmp_four_set_len)
                                        self.train_u_b_t_cnt_ += tmp_four_train_set_len
                                        self.test_u_b_t_cnt_ += tmp_four_set_len - tmp_four_train_set_len

                                        for item in tmp_four_set[0:tmp_four_train_set_len]:
                                            tf.write(str(item[0])+"\t"+str(item[2])+"\t"+str(item[3])+"\n")
                                            self.train_user_set_.add(item[0])
                                            self.train_bookmark_set_.add(item[2])
                                            self.train_tag_set_.add(item[3])

                                        for item in tmp_four_set[tmp_four_train_set_len:]:
                                            tft.write(str(item[0])+"\t"+str(item[2])+"\t"+str(item[3])+"\n")
                                            self.test_user_set_.add(item[0])
                                            self.test_bookmark_set_.add(item[2])
                                            self.test_tag_set_.add(item[3])

                                    else:
                                        print "error" 

                                tmp_four_set = []
                                tmp_four_set.append(self.u_b_t_list_[idx])
                            else:
                                tmp_four_set.append(self.u_b_t_list_[idx])

                        
                        tmp_four_set_len = len(tmp_four_set)

                        if tmp_four_set_len > self.degree_filter_param_:

                            tmp_four_train_set_len = int(0.8 * tmp_four_set_len)
                            self.train_u_b_t_cnt_ += tmp_four_train_set_len
                            self.test_u_b_t_cnt_ += tmp_four_set_len - tmp_four_train_set_len

                            for item in tmp_four_set[0:tmp_four_train_set_len]:
                                tf.write(str(item[0])+"\t"+str(item[2])+"\t"+str(item[3])+"\n")
                                self.train_user_set_.add(item[0])
                                self.train_bookmark_set_.add(item[2])
                                self.train_tag_set_.add(item[3])

                            for item in tmp_four_set[tmp_four_train_set_len:]:
                                tft.write(str(item[0])+"\t"+str(item[2])+"\t"+str(item[3])+"\n")
                                self.test_user_set_.add(item[0])
                                self.test_bookmark_set_.add(item[2])
                                self.test_tag_set_.add(item[3])

                            tmp_four_set = []

                        else:
                            print "error"
                        
                except Exception,e: 
                    print str(e)
                    print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
                    exit(-1)

        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)


        try:
            with open(self.data_dst_dir_ + "/" + self.readme_file_ , "w") as tf:
                tf.write("total_user : " + str(len(self.user_.obj_cnt_map_)) + "\n")
                tf.write("total_bookmarks : " + str(len(self.bookmarks_.obj_cnt_map_)) + "\n")
                tf.write("total_tags : " + str(len(self.tags_.obj_cnt_map_)) + "\n")
                tf.write("total_user_bookmarks_tags : " + str(len(self.u_b_t_list_)) + "\n")
                tf.write("\n\n\n")
                tf.write("train_user : " + str(len(self.train_user_set_)) + "\n")
                tf.write("train_bookmarks : " + str(len(self.train_bookmark_set_)) + "\n")
                tf.write("train_tags : " + str(len(self.train_tag_set_)) + "\n")
                tf.write("train_user_bookmarks_tags : " + str(self.train_u_b_t_cnt_) + "\n")
                tf.write("\n\n\n")
                tf.write("test_user : " + str(len(self.test_user_set_)) + "\n")
                tf.write("test_bookmarks : " + str(len(self.test_bookmark_set_)) + "\n")
                tf.write("test_tags : " + str(len(self.test_tag_set_)) + "\n")
                tf.write("test_user_bookmarks_tags : " + str(self.test_u_b_t_cnt_) + "\n")
                tf.write("\n\n\n")

        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)


        try:
            with open(self.data_dst_dir_ + "/" + self.b_file_ , "w") as tf:
                for (k,v) in self.bookmarks_.bookmarks_value_map_.items():
                    tf.write(str(k) + '\n')
                    tf.write(str(v) + '\n')
        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)


        try:
            with open(self.data_dst_dir_ + "/" + self.t_file_ , "w") as tf:
                for (k,v) in self.tags_.tags_value_map_.items():
                    tf.write(str(k) + '\n')
                    tf.write(str(v) + '\n')
        except Exception,te:
            print str(te)
            print str(inspect.currentframe().f_code.co_filename) + ":" + str(inspect.currentframe().f_lineno)
            exit(-1)

        print "generate data successfully!"


if __name__ == "__main__":

    deli = data_wash("../../data_src/delicious",                \
                     "../../data_dst/delicious",                \
                     "user_taggedbookmarks-timestamps.dat",     \
                     "tags.dat",                                \
                     "bookmarks.dat",
                     "Readme")

    deli.handler_data()

    deli.generate_data()
