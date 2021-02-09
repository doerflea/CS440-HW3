#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>


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

int hash(int n){
   //n num buckets
   return 0 % n;
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
	 std::unordered_map<int,int>mp(readBucketArray());
	 std::string tuple;
	 while(std::getline(emp_file, tuple)){
	    std::stringstream ss(tuple);
	    std::string id;
	    ss >> id;

	    std::cout << "id: " << id << std::endl;
	   
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
