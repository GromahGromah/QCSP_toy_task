#include "algo.h"
#include "vessel.h"
#include "plan.h"

#include <cstdio>
#include <algorithm>

int main()
{
	Vessel vessel("vessel_instances/shanghai_2.vessel");
	CraneWorkingPlan plan = DPSolve(vessel).GetMergeMovingActions();
	plan.Print();
	BayPlan bay_plan = BayPlan::ExtractBayPlan(vessel, plan);
	FILE * fout = fopen("bay_plan.txt", "w");
	bay_plan.Save(fout);
	return 0;
}
