
// Copyright 2018 Your Name <your_email>
#include "header.hpp"
int main(int argc, char *argv[]) {
  path path_to_dir;
  if (argc == 1){
    std::string str(argv[0]);
    str += "../../misc/ftp";
      path_to_dir = path{str};
  }else{
      path_to_dir = path{argv[1]};
  }
  Parser obj(path_to_dir);
  obj.ParsingDir();
  return 0;
}
