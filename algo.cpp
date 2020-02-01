#include "algo.h"

DPConfig::DPConfig()
	: DPConfig(10, 1) {
}

DPConfig::DPConfig(int batch_size, int thread_count)
	: batch_size(batch_size), thread_count(thread_count) {
}

CraneWorkingPlan DPSolve(const Vessel &vessel, const DPConfig &config) {
	/* TODO */
	return CraneWorkingPlan();
}
