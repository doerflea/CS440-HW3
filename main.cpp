#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <math.h>
#include <bitset>
#include <functional>

/***************************************
 * *Get hash code and flip ith bit, used for when bucket is full
 * *i: ith bit to extract upto and flip from hash value
 * *id: employee id to used to get hash value
 *******************************************/
int hash(int i, std::string id){
    std::hash<std::string> string_hash;
    int hash_code = string_hash(id);
    //get last i bits
    int last_i_bits = hash_code & ((1 << i-1) - 1);
    return last_i_bits;
}


/***************************************
 * *Get hash code
 * *i: ith bit to extract upto from hash value
 * *id: employee id to used to get hash value
 *******************************************/
int hash_flip(int i, std::string id){
    //std::cout << "i: " << i << std::endl;
    std::hash<std::string> string_hash;
    int hash_code = string_hash(id);
    //get last i bits
    int last_i_bits = hash_code & ((1 << i) - 1);
    int flip = last_i_bits ^ (1 << i-1);
    return last_i_bits;
}


/***************************************
 * *Appends entry to bucket file if it is not full
 * *bucket_id: hashed id of file
 *********************************************/
bool append_entry(int bucket_id){
   //Try to add entry if bucket is not full


   //return true if success, false if was full
   return false;

}
/***************************************
 * *Trys to append entry to 1st original hashed bucket,
    then flipped hashed bucket, if everything if full make overflow bucket
 * *bucket_id: hashed id of file
 * *i: current ith bit
 *********************************************/
void add_entry(std::string id, int i){
    int bucket_id = hash(i, id);
    bool added_bucket = append_entry(bucket_id);
    if(added_bucket == false){
      bucket_id = hash_flip(i, id);
   }
   else{
      return;
   }
   added_bucket = append_entry(bucket_id);
   if(added_bucket == false){
      //Make overflow bucket
   }

}
/***************************************
 * *Splits bucket, rehashing values
 * *next_split: id of next bucket to split
 *******************************************/
void split(int next_split, int i){
    //values in bucket that gets split gets rehashed
}

/***************************************
 * *Gets bucket array from file that maintains(h<key>, bucket_id>
 *******************************************/
std::unordered_map<int,int> readBucketArray(){
    std::unordered_map<int,int>mp;
    
    //File with bucket array that maintains bucket_array;
    std::ifstream bucket_array;
    bucket_array.open("bucket_array.txt");
    
    if(bucket_array.is_open()){
        //read file into mp
        std::string pair;
        while(std::getline(bucket_array, pair)){
            std::stringstream ss(pair);
            std::string key, bucket_id;
            ss >> key >> bucket_id;
            mp[std::stoi(key)] = std::stoi(bucket_id);
        }
        bucket_array.close();
    }
    return mp;
}


int main(int argc, char *argv[]){
    if(argc < 2){
        std::cout << "Invalid arguments" << std::endl;
        return 0;
    }
    //Look up mode, must include employee
    if(strcmp(argv[1],"L")  == 0 && argc == 3){
        std::cout << "lookup " << argv[2] << std::endl;
        std::unordered_map<int,int> mp(readBucketArray());
        if(mp.size() == 0){
            std::cout << "Employee not found" << std::endl;
        }
        else{
            //get hash code
            //look up record file bucket id in mp
        }
        return 0;
    }
    //Creation mode
    else if(strcmp(argv[1],"C") == 0){
        std::cout << "creation" << std::endl;
        
        //File of employees (id, name, bio, manager-id)
        std::ifstream emp_file;
        emp_file.open("Employees.csv");
        
        if(emp_file.is_open()){
            //number of buckets
            int prev_n = 2;
            int n = 2;
            int d = 4; //number of items per bucket, need to calculate just placeholder
            int next_split = 0;
            int i = 1;
            int records = 0;
            
            //number of splits
            int rounds = 0;
            
            std::string tuple;
            while(std::getline(emp_file, tuple)){
                //When average nummber of records exceeds 80% of block capacity,
                if((float)records/(n * d) >= .80){
                    split(next_split, i);
                    //increment next bucket to split pointer
                    next_split++;
                    //add bucket
                    n++;
                    //increment number of rounds(splits)
                    rounds++;
                    if(rounds == prev_n){
                        prev_n = n;
                        next_split = 0;
                        i++;
                    }
                }
                std::stringstream ss(tuple);
                std::string id;
                ss >> id;
                int bucket_hash_code = hash(i, id);
                int flip = hash_flip(i, id);
                records++;
                
            }
            
            emp_file.close();
        }
        else{
            std::cout << "Invalid file name" << std::endl;
        }
    }
    else{
        std::cout << "Invalid arguments" << std::endl;
    }
    return 0;
}
