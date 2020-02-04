#ifndef ALGO_H
#define ALGO_H

#include "vessel.h"

struct DPConfig {
	static constexpr int kMaxCraneCount = 25;
	static constexpr int kMaxBatchCount = 400;
	static constexpr int kMannerCount = 4;
	static constexpr int kDefaultBatchSize = 25;
	int batch_size;
	DPConfig();
	DPConfig(int batch_size);
};

CraneWorkingPlan DPSolve(const Vessel &vessel, const DPConfig &config = DPConfig());

#endif /* ALGO_H */
