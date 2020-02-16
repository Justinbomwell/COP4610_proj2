#include <unistd.h>

int main()
{
  int i;
  for(i = 0; i < 5; i++)
  {
    access("/home", F_OK);
  }
  /* code */
  return 0;
}
