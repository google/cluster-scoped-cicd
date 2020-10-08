// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ctime>
#include <string>
#include <iostream>

//#include <git2.h>

std::string get_greet(const std::string& who) {
  return "Ahoy! " + who;
}

time_t return_gittime(time_t git_time) {
  std::time_t temp = git_time;
  std::cout << ctime(&temp);
  return temp;
}

time_t return_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
  return result;
}

void print_time_delta(time_t arg1, time_t arg2) {
  time_t delta;
  char delta_str[100];
  tm* delta_tm;

  delta = difftime(arg1, arg2);
  delta_tm = localtime(&delta);
  strftime(delta_str, 100, "%x %X", delta_tm);
  std::cout << "Time Delta (Execution - Commit): " << delta << " | " << delta_str << std::endl;
}

int main(int argc, char** argv) {
  //git_libgit2_init();

  /*int rc;
  const char* REPO_PATH = "http://github.com/jmahboob/angular-js-test";
  const char* PATH = "./tmp";
  git_repository* repo = NULL;
  std::cout << "test" << std::endl;
  int error = git_clone(&repo, REPO_PATH, PATH, NULL);
  git_repository_free(repo);

  git_commit* commit = NULL;
  git_oid oid;
  git_repository_open(&repo, PATH);
  rc = git_reference_name_to_id(&oid, repo, "HEAD");
  rc = git_commit_lookup(&commit, repo, &oid);

  //std::cout << git_commit_time(commit) << std::endl;
  std::time_t localtime = return_localtime();
  std::time_t gittime = return_gittime( git_commit_time(commit) );
  print_time_delta( localtime, gittime );*/

  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }
  std::cout << get_greet(who) << std::endl;
  return 0;
}
