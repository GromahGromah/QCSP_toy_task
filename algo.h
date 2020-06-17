#ifndef ALGO_H
#define ALGO_H

#include "plan.h"
#include "vessel.h"

struct DPConfig {
	static constexpr int kMaxCraneCount = 25;
	static constexpr int kMaxBatchCount = 400;
	static constexpr int kMannerCount = 4;
	static constexpr int kDefaultBatchSize = 20;
	static constexpr int kDefaultThreadCount = 1;
	int batch_size, thread_count;
	DPConfig();
	DPConfig(int batch_size);
	DPConfig(int batch_size, int thread_count);
};

CraneWorkingPlan DPSolve(const Vessel &vessel, const DPConfig &config = DPConfig());

#endif /* ALGO_H */
