#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <math.h>
#include <bitset>
#include <functional>


/***************************************
 * * stored unordered_map to bucket array
 *******************************************/
void store_bucket_array(std::unordered_map<unsigned long, unsigned long>&mp){
    std::ofstream bucket_array;
    bucket_array.open("bucket_array.txt");
    for(auto p : mp){
        bucket_array << p.first << " " << p.second << "\n";
    }
    bucket_array.close();
}
    
/***************************************
 * *Get hash code and flip ith bit, used for when bucket is full
 * *i: ith bit to extract upto and flip from hash value
 * *id: employee id to used to get hash value
 *******************************************/
unsigned long hash(int i, std::string id, unsigned long &full_hash){
    std::hash<std::string> string_hash;
    unsigned long hash_code = string_hash(id);
    full_hash = hash_code;
    //get last i bits
    unsigned long last_i_bits = hash_code & ((1 << i) - 1);
    return last_i_bits;
}

/***************************************
 * *Get hash code
 * *i: ith bit to extract upto from hash value
 * *id: employee id to used to get hash value
 *******************************************/
unsigned long hash_flip(int i, std::string id){
    std::hash<std::string> string_hash;
    unsigned long hash_code = string_hash(id);
    
    //get last i bits
    unsigned long last_i_bits = hash_code & ((1 << i) - 1);
    
    //flip ith bit
    unsigned long flip = last_i_bits ^ (1 << (i-1));
    return flip;
}

/***************************************
 * *Appends entry to bucket file if it is not full
 * *bucket_id: hashed id of file
 *********************************************/
bool append_entry(unsigned long bucket_id, std::string id, std::string record, int record_size){
    //bucket file name
    std::streampos begin, end;
    std::string file = std::to_string(bucket_id) + ".txt";
    std::ifstream bucket_file;
    //Try to add entry if bucket is not full
    bucket_file.open(file, std::ios::binary);
    
    //Get size of file
    if(bucket_file.is_open()){
        begin = bucket_file.tellg();
        bucket_file.seekg(0, std::ios::end);
        end = bucket_file.tellg();
        int n = (end - begin);
        bucket_file.close();
        //record won't fit in this bucket
        if(n + record_size > 4096){
            return false;
        }
    }
    //open file, or create if it does not yet exist, to append record
    std::ofstream bucket;
    bucket.open(file, std::ios::app);
    bucket << id << "," << record << "\n";
    bucket.close();
    
    return true;

}
/***************************************
 * *Trys to append entry to 1st original hashed bucket,
    then flipped hashed bucket, if everything if full make overflow bucket
 * *bucket_id: hashed id of file
 * *i: current ith bit
 *********************************************/
void add_entry(std::string id, int i, std::string record, std::unordered_map<unsigned long, unsigned long>&mp){
    //size of the record
    
    //+2 for newline character and comma
    int size = id.length() + record.length() + 2;
    
    unsigned long full_hash = 0;
    unsigned long bucket_id = hash(i, id, full_hash);
    bool added_bucket = append_entry(bucket_id, id, record, size);
    if(added_bucket == false){
      bucket_id = hash_flip(i, id);
   }
   else{
       //Sucessfully added to first hash
       mp.insert({full_hash, bucket_id});
      return;
   }
   added_bucket = append_entry(bucket_id, id, record, size);
   if(added_bucket == false){
      //Make overflow bucket
   }
   else{
       //Sucessfully added to flipped hash
       mp.insert({full_hash, bucket_id});
   }

}
/***************************************
 * *Splits bucket, rehashing values
 * *next_split: id of next bucket to split
 *******************************************/
void split(int next_split, int i){
    //values in bucket that gets split gets rehashed
    //If bucket is full, then next to checked flipped bit bucket
}

/***************************************
 * *Gets bucket array from file that maintains(h<key>, bucket_id>
 *******************************************/
std::unordered_map<unsigned long ,unsigned long> readBucketArray(){
    std::unordered_map<unsigned long, unsigned long>mp;
    
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
        std::unordered_map<unsigned long, unsigned long> mp(readBucketArray());
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
        std::unordered_map<unsigned long, unsigned long>mp;
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
                std::string id, record;
                std::getline(ss, id, ',');
                std::getline(ss, record, '\n');
                add_entry(id, i, record, mp);
                records++;
            }
            emp_file.close();
            store_bucket_array(mp);
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
