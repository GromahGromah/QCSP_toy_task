#ifndef ALGO_H
#define ALGO_H

#include "vessel.h"

struct DPConfig {
	static constexpr int kMaxCraneCount = 25;
	static constexpr int kMaxBatchCount = 400;
	static constexpr int kMannerCount = 4;
	int batch_size;
	int thread_count;
	DPConfig();
	DPConfig(int batch_size, int thread_count);
};

CraneWorkingPlan DPSolve(const Vessel &vessel, const DPConfig &config);

#endif /* ALGO_H */
