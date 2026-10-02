#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>  
#include <cstdio>
#include <string>

size_t sendMsg(int& err, int wr, const char* b, size_t k)
{
  err = 0;
  ssize_t sent = write(wr, &k, sizeof(k));
  if (sent != sizeof(k))
  {
    err = -1;
    return 0;
  }

  
  size_t total = 0;
  while (total < k)
  {
    ssize_t n = write(wr, b + total, k - total);
    if (n <= 0)
    {
      err = -1;
      return total;
    }
    total += n;
  }
  return total;
}

int main() {
  int pps[2];
  if (pipe(pps) == -1)
  {
    perror("pipe"); return 1;
  }
  
  int rd = pps[0], wr = pps[1];
  pid_t pid = fork();
  
  if (pid == -1)
  { 
    perror("fork"); return 1;
  }

  if (pid == 0) 
  {
    close(wr);
    char p[20];
    snprintf(p, sizeof(p), "%d", rd);
    execl("./child", "child", p, NULL);
    perror("execl failed"); 
    return 1;
  }
  
  close(rd);
  
  std::string msg;
  std::getline(std::cin, msg);
  
  int err = 0;
  sendMsg(err, wr, msg.c_str(), msg.size());
  if (err < 0)
  {
    std::cerr << "Error sending data to child!" << "\n";
  }
  
  close(wr);
  waitpid(pid, NULL, 0);
  return 0;
}