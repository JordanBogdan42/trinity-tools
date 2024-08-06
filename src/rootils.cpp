#include "rootils.h"

#include "constants.h"

#include <Event.h>

void SetBranches(Event **evD)
{
	tree->SetBranchAddress("Events", evD);
}