#include "algo.h"
#include "vessel.h"
#include "plan.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>

int main()
{
	Vessel vessel("vessel_instances/shanghai_2.vessel");
	CraneWorkingPlan plan = DPSolve(vessel).GetMergeMovingActions();
	plan.Print();
	BayPlan bay_plan = BayPlan::ExtractBayPlan(vessel, plan);
	FILE * fout = fopen("bay_plan.txt", "w");
	bay_plan.Print(fout);
	fclose(fout);
	system("python draw.py bay_plan.txt");
	return 0;
}
