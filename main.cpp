#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <math.h>
#include <bitset>
#include <functional>



void split(int next_split, int i){
   //values in bucket that gets split gets rehashed
}


std::unordered_map<int,int> readBucketArray(){
   std::unordered_map<int,int>mp;

   //File with bucket array that maintains <h(key), bucket id>
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

int hash(int i, std::string id){
   std::cout << "i: " << i << std::endl;
   std::hash<std::string> string_hash;
   int hash_code = string_hash(id);
   std::cout << "raw hash code: " << hash_code << std::endl;
   //get last i bits
   int last_i_bits = hash_code & ((1 << i) - 1);
   std::cout << "ith bits: " <<  last_i_bits << std::endl;
  
   
   
   //look up i most sig bits
   //try to flip ith most sig bits if bucket is full

   //hash id, mod by 2^i to determine bucket
   int mod = pow(2, i);
   return 0 % mod;
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
	    records++;

	    //std::cout << "id: " << id << std::endl;
	   
	    //get hash code using id, read out to bucket, when buckets are 80% full add bucket, use bits ??
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
