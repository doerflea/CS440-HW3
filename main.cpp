#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char *argv[]){
   if(argc < 2){
      std::cout << "Invalid arguments" << std::endl;
      return 0;
   }
   //Look up mode, must include employee
   if(strcmp(argv[1],"L")  == 0 && argc == 3){
      std::cout << "lookup " << argv[2] << std::endl;

      //file with bucket array that maintains <h(key), bucket id>
      std::ifstream bucket_array;
      bucket_array.open("bucket_array.txt");
      if(bucket_array.is_open()){
	 bucket_array.close();
      }
      else {
	 std::cout << "Employee not found" << std::endl;
      }
   }
   //Creation mode
   else if(strcmp(argv[1],"C") == 0){
      std::cout << "creation" << std::endl;

      //File of employees (id, name, bio, manager-id)
      std::ifstream emp_file;
      emp_file.open("Employees.csv");

      if(emp_file.is_open()){
	 std::string tuple;
	 while(std::getline(emp_file, tuple)){
	    std::cout << tuple << std::endl;
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
