#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <math.h>
#include <bitset>
#include <functional>
#include <stdio.h>

/***************************************
 * *Appends entry to bucket file if it is not full
 * *bucket_id: hashed id of file
 *********************************************/
bool append_entry(std::string file_name, std::string id, std::string record, int record_size){
    //bucket file name
    std::streampos begin, end;
    std::ifstream bucket_file;
    //Try to add entry if bucket is not full
    bucket_file.open(file_name, std::ios::binary);
    
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
    bucket.open(file_name, std::ios::app);
    bucket << id << "," << record << "\n";
    bucket.close();
    
    return true;

}
/***************************************
 * * check if bucket has overflow bucket made
 *******************************************/
bool check_overflow (std::string file_name){
    std::ifstream bucket_file;
    bucket_file.open(file_name);
    if(bucket_file.is_open()) {
        //Go to end of file and read until "/n"
        bucket_file.seekg(-1, std::ios_base::end);
        if(bucket_file.peek() == '\n'){
            bucket_file.seekg(-1, std::ios_base::cur);
            int i = bucket_file.tellg();
            for(i; i > 0; i--) {
                if(bucket_file.peek() == '\n'){
                    //Found
                    bucket_file.get();
                    break;
                }
                bucket_file.seekg(i, std::ios_base::beg);
            }
        }
        std::string lastline;
        getline(bucket_file, lastline);
        if(lastline[0] == 'O'){
            return true;
        }
    }
    return false;
}

/***************************************
 * *add to overflow bucket
 *******************************************/
void add_to_overflow(std::string id, std::string record, int record_size, std::string filename, std::unordered_map<std::string, std::string>&mp, long long full_hash){
    bool overflowed = check_overflow(filename);
    //recursively checking overflow buckets
    if(overflowed){
        add_to_overflow(id, record, record_size, "O" + filename, mp, full_hash);
    }
    else{
        bool appended = append_entry(filename, id, record, record_size);
        if(appended){
            //sucessfully added, store to bucket array
            if(mp.find(std::to_string(full_hash)) != mp.end()){
                mp[std::to_string(full_hash)] = filename;
            }
            else{
                mp.insert({std::to_string(full_hash), filename});
            }
        }
        //chain another overflow bucket
        else{
            std::string org_file = filename;
            std::string file_name = "O" + filename;
            std::ofstream bucket;
            bucket.open(file_name, std::ios::app);
            bucket << id << "," << record << "\n";
            bucket.close();
            if(mp.find(std::to_string(full_hash)) != mp.end()){
                mp[std::to_string(full_hash)] = file_name;
            }
            else{
                mp.insert({std::to_string(full_hash), file_name});
            }
            bucket.open(org_file, std::ios::app);
            bucket << file_name << "\n";
            bucket.close();
        }
    }
}

/***************************************
 * * stored unordered_map to bucket array
 *******************************************/
void store_bucket_array(std::unordered_map<std::string, std::string>&mp, int n, int i, int next_split){
    std::ofstream bucket_array;
    bucket_array.open("bucket_array.txt");
    bucket_array << n << " " << i << " " << next_split << "\n";
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
 * *Trys to append entry to 1st original hashed bucket,
    then flipped hashed bucket, if everything if full make overflow bucket
 * *bucket_id: hashed id of file
 * *i: current ith bit
 *********************************************/
void add_entry(std::string id, int i, std::string record, std::unordered_map<std::string, std::string>&mp){
    //size of the record
    
    //+2 for newline character and comma
    int size = id.length() + record.length() + 2;
    
    unsigned long full_hash = 0;
    unsigned long bucket_id = hash(i, id, full_hash);
    bool overflow_already_made = check_overflow(std::to_string(bucket_id) + ".txt");
    if(overflow_already_made){
        std::string overflow_name = "O" + std::to_string(bucket_id) + ".txt";
        add_to_overflow(id, record, size, overflow_name, mp, full_hash);
        return;
    }
    bool added_bucket = append_entry(std::to_string(bucket_id) + ".txt", id, record, size);
    unsigned long bucket_id_flip = 0;
    if(added_bucket == false){
     bucket_id_flip = hash_flip(i, id);
   }
   else{
       //Sucessfully added to first hash
       if(mp.find(std::to_string(full_hash)) != mp.end()){
           mp[std::to_string(full_hash)] = std::to_string(bucket_id) + ".txt";
       }
       else{
           mp.insert({std::to_string(full_hash), std::to_string(bucket_id) + ".txt"});
       }
       return;
   }
    added_bucket = append_entry(std::to_string(bucket_id_flip) + ".txt", id, record, size);
   if(added_bucket == false){
      //Make overflow bucket
       std::string org_file = std::to_string(bucket_id) + ".txt";
       std::string file_name = "O" + std::to_string(bucket_id) + ".txt";
       std::ofstream bucket;
       bucket.open(file_name, std::ios::app);
       bucket << id << "," << record << "\n";
       bucket.close();
       //add overflow bucket to orginal file
       bucket.open(org_file, std::ios::app);
       bucket << file_name << "\n";
       bucket.close();
       //Sucessfully added to overflow bucket
       if(mp.find(std::to_string(full_hash)) != mp.end()){
           mp[std::to_string(full_hash)] = std::to_string(bucket_id) + ".txt";
       }
       else{
           mp.insert({std::to_string(full_hash), std::to_string(bucket_id) + ".txt"});
       }
       return;
   }
   else{
       //Sucessfully added to flipped hash
       if(mp.find(std::to_string(full_hash)) != mp.end()){
           mp[std::to_string(full_hash)] = std::to_string(bucket_id_flip) + ".txt";;
       }
       else{
           mp.insert({std::to_string(full_hash), std::to_string(bucket_id_flip) + ".txt"});
       }
       
   }

}
/***************************************
 * *Splits bucket, rehashing values
 * *next_split: id of next bucket to split
 *******************************************/
void split(int next_split, int i, std::unordered_map<std::string, std::string>&mp){
    //values in bucket that gets split gets rehashed
    //If bucket is full, then next to checked flipped bit bucket
    std::streampos begin, end;
    std::string file = std::to_string(next_split) + ".txt";
    std::ifstream bucket_file;
    //rehash overflow buckets
    while(check_overflow(file)){
        std::string old_file = file;
        file = "O" + file;
        if(old_file[0] == 'O'){
            if (std::rename(old_file.c_str(), "temp.txt") != 0){
                perror("Error renaming file");
            }
            //delete old file
            if (remove("temp.txt") !=0){
                std::cout << "Remove operation failed" << std::endl;
            }
        }
        std::ifstream overflow_file;
        overflow_file.open(file);
        if(overflow_file.is_open()){
            std::string tuple;
            while(std::getline(overflow_file, tuple)){
                std::stringstream ss(tuple);
                std::string id, record;
                std::getline(ss, id, ',');
                std::getline(ss, record, '\n');
                add_entry(id, i, record, mp);
            }
            overflow_file.close();
        }
    }
    bucket_file.open(file);
    //rehash entries file
    if(bucket_file.is_open()){
        //rename file to temp.txt
        if (std::rename(file.c_str(), "temp.txt") != 0){
            perror("Error renaming file");
        }
        std::string tuple;
        while(std::getline(bucket_file, tuple)){
            //No record name of overflow bucket
            if(tuple[0] == 'O'){
                break;
            }
            std::stringstream ss(tuple);
            std::string id, record;
            std::getline(ss, id, ',');
            std::getline(ss, record, '\n');
            add_entry(id, i, record, mp);
        }
        bucket_file.close();
        if (remove("temp.txt") !=0){
            std::cout << "Remove operation failed" << std::endl;
        }
    }
    else{
        //empty
        return;
    }
    //Get size of file
    bucket_file.open(file, std::ios::binary);
    if(bucket_file.is_open()){
        begin = bucket_file.tellg();
        bucket_file.seekg(0, std::ios::end);
        end = bucket_file.tellg();
        int n = (end - begin);
        bucket_file.close();
        //Max size of a record is 716, so if 4096 - record size < 716, there are possibly values in flipped bit bucket
        if((4096 - n) < 716){
            //check flipped bit bucket
        }
    }
}

/***************************************
 * *Gets bucket array from file that maintains(h<key>, bucket_id>
 *******************************************/
std::unordered_map<std::string, std::string> readBucketArray(int &n, int &i, int &next_split){
    std::unordered_map<std::string, std::string>mp;
    
    //File with bucket array that maintains bucket_array;
    std::ifstream bucket_array;
    bucket_array.open("bucket_array.txt");
    
    if(bucket_array.is_open()){
        //read file into mp
        bool read_n_i = false;
        std::string pair;
        while(std::getline(bucket_array, pair)){
            std::stringstream ss(pair);
            std::string key, bucket_id;
            ss >> key >> bucket_id;
            //store n and i on first line of file
            if(!read_n_i){
                n = stoi(key);
                i = stoi(bucket_id);
                std::string split;
                ss >> split;
                next_split = stoi(split);
                read_n_i = true;
                continue;
            }
            mp.insert({key, bucket_id});
        }
        bucket_array.close();
    }
    return mp;
}
/***************************************
 * *Look up employee
 *********************************************/
void look_up(std::string id){
    int n, i, next_split;
    std::unordered_map<std::string, std::string> mp(readBucketArray(n, i, next_split));
    std::hash<std::string> string_hash;
    unsigned long hash_code = string_hash(id);
    std::string key = std::to_string(hash_code);
    //Didn't find hash of id
    if(mp.size() == 0 || mp.find(key) == mp.end()){
        std::cout << "Employee not found" << std::endl;
    }
    else{
        std::ifstream bucket_file;
        std::string file_name = mp[key];
        bucket_file.open(file_name);
        if(bucket_file.is_open()){
            std::string tuple;
            while(std::getline(bucket_file, tuple)){
                std::stringstream ss(tuple);
                std::string emp_id, record;
                std::getline(ss, emp_id, ',');
                std::getline(ss, record, '\n');
                if(emp_id == id){
                    std::cout << id << ": " << record << std::endl;
                    bucket_file.close();
                    return;
                }
            }
        }
        bucket_file.close();
        while(check_overflow(file_name)){
            file_name = "O" + file_name;
            std::ifstream overflow_file;
            overflow_file.open(file_name);
            if(overflow_file.is_open()){
                std::string tuple;
                while(std::getline(overflow_file, tuple)){
                    std::stringstream ss(tuple);
                    std::string emp_id, record;
                    std::getline(ss, emp_id, ',');
                    std::getline(ss, record, '\n');
                    if(emp_id == id){
                        std::cout << id << ": " << record << std::endl;
                        overflow_file.close();
                        return;
                    }
                }
            }
            overflow_file.close();
        }
    }
}

int main(int argc, char *argv[]){
    if(argc < 2){
        std::cout << "Error: Invalid arguments." << std::endl;
        return 0;
    }
    //Look up mode, must include employee
    if(strcmp(argv[1],"L")  == 0 && argc == 3){
        std::cout << "lookup " << argv[2] << std::endl;
        look_up(argv[2]);
        return 0;
    }
    //Creation mode
    else if(strcmp(argv[1],"C") == 0){
        std::cout << "creation" << std::endl;
        //File of employees (id, name, bio, manager-id)
        std::ifstream emp_file;
        emp_file.open("Employee.csv");
        
        if(emp_file.is_open()){
            //number of buckets
            int n = 2;
            int i = 1;
            int next_split = 0;
            std::unordered_map<std::string, std::string>mp = readBucketArray(n, i, next_split);
            int prev_n = 2;
            int d = 5; //max number of items per bucket
            int records = mp.size();
            
            //number of splits
            int rounds = 0;
            
            std::string tuple;
            while(std::getline(emp_file, tuple)){
                //When average nummber of records exceeds 80% of block capacity,
                if((float)records/(n * d) >= .80){
                    split(next_split, i, mp);
                    //increment next bucket to split pointer
                    next_split++;
                    //add bucket
                    n++;
                    //increment number of rounds(splits)
                    rounds++;
                    if(rounds == prev_n){
                        prev_n = n;
                        next_split = 0;
                        if((pow(2,i)-1 < n)){
                            i++;
                        }
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
            store_bucket_array(mp, n, i, next_split);
        }
        else{
            std::cout << "Error: Employee.csv does not exist." << std::endl;
        }
    }
    else{
        std::cout << "Error: Invalid arguments." << std::endl;
    }
    return 0;
}
