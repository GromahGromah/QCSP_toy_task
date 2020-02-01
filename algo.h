#ifndef ALGO_H
#define ALGO_H

#include "vessel.h"

struct DPConfig {
	int batch_size;
	int thread_count;
	DPConfig();
	DPConfig(int batch_size, int thread_count);
};

CraneWorkingPlan DPSolve(const Vessel &vessel, const DPConfig &config);

#endif /* ALGO_H */
