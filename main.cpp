#include "vessel.h"
#include "algo.h"

#include <cstdio>
#include <algorithm>

int main()
{
	Vessel vessel("vessel_instances/shanghai_2.vessel");
	CraneWorkingPlan plan = DPSolve(vessel).GetMergeMovingActions();
	plan.Print();
	return 0;
}
