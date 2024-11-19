#include "DeadlockResolution.h"
#include <stdio.h>
#include <stdlib.h>

/*
You'll need subfunction here.
It is advised to make at least one recursive subfunction whose role is to explore all possible paths (valid), and one that is charged to verify that a given path of size bound cannot be extended.
You could call the last one 'verifier', but that is a slight abuse: as thus we should consider every possible step array, even those which are not real execution.
To optimize, you should ensure that the 'verifier' only gets real executions (said otherwise, it doesn't verify everything) -- you'll thus get a way more efficient algorithm.
*/

bool deadlock_brute_force(LockAutomaton *automata, int num_automata, int bound, step *path)
{
  return false;
}