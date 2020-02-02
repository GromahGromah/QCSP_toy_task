#include "algo.h"

#include <mutex>
#include <thread>
#include <cassert>
#include <algorithm>

#define T_STATIC thread_local static
#define INF 1e30
using Pii = std::pair<int, int>;

DPConfig::DPConfig()
	: DPConfig(10, 1) {
}

DPConfig::DPConfig(int batch_size, int thread_count)
	: batch_size(batch_size), thread_count(thread_count) {
}

static inline int CeilDiv(int lhs, int rhs) {
	return (lhs + rhs - 1) / rhs;
}

CraneWorkingPlan DPSolve(const Vessel &vessel, const DPConfig &config) {
	/* Initialize */
	int batch_sz = std::max(config.batch_size, CeilDiv(vessel.GetTotalTEU(), DPConfig::kMaxBatchCount));
	int cranes = int(vessel.cranes.size());
	int batches = 0;
	int manners = DPConfig::kMannerCount;
	assert(cranes <= DPConfig::kMaxCraneCount);
	
	std::vector<int> batches_per_bay, prefix_batches;
	for (int teu : vessel.teus_per_bay) {
		int batch_num = CeilDiv(teu, batch_sz);
		batches_per_bay.push_back(batch_num);
		prefix_batches.push_back((prefix_batches.empty() ? 0 : prefix_batches.back()) + batch_num);
		batches += batch_num;
	}
	
	std::vector<int> crane_ord(cranes);
	for (int c = 0; c < cranes; c ++)
		crane_ord[c] = c;
	std::sort(crane_ord.begin(), crane_ord.end(), [&](int lhs, int rhs) {
		return vessel.cranes[lhs].init_pos < vessel.cranes[rhs].init_pos;
	});
	
	std::vector<int> batch_pos;
	for (int b = 0; b < int(batches_per_bay.size()); b ++)
		for (int i = 0; i < batches_per_bay[b]; i ++)
			batch_pos.push_back(b);
	
	T_STATIC double Dp[DPConfig::kMaxCraneCount][DPConfig::kMannerCount][DPConfig::kMaxBatchCount][DPConfig::kMaxBatchCount];
	T_STATIC Pii Fa[DPConfig::kMaxCraneCount][DPConfig::kMannerCount][DPConfig::kMaxBatchCount][DPConfig::kMaxBatchCount];
	/* Solve */
	struct Index {
		int crane, manner, l, r;
		Index(int crane, int manner, int l, int r)
			: crane(crane), manner(manner), l(l), r(r) {
		}
	};
	auto Calc = [&](const Index &index) -> double {
		double ret = 0.0;
		// TODO : Complete Code
		return ret;
	};
	auto Clash = [&](const Index &lhs, const Index &rhs) -> bool {
		bool ret = false;
		// TODO : Complete Code
		return ret;
	};
	for (int c = 0; c < cranes; c ++)
		for (int m = 0; m < manners; m ++) {
			int l_st = c, l_ed = (c == 0 ? 0 : batches - c - 1);
			for (int l = l_st; l <= l_ed; l ++)
				for (int r = l; r < batches - c; r ++) {
					Index cur_index = Index(crane_ord[c], m, l, r);
					double ti = Calc(cur_index);
					if (c == 0)
						Dp[c][m][l][r] = ti;
					else {
						double &min_val = Dp[c][m][l][r];
						int &fa_manner = Fa[c][m][l][r].first;
						int &fa_pos = Fa[c][m][l][r].second;
						min_val = INF, fa_manner = -1, fa_pos = -1;
						
						std::mutex mutex;
						std::vector<std::thread> threads;
						/* Using Static Parallel Strategy */
						for (int tid = 0; tid < config.thread_count; tid ++)
							threads.emplace_back([&]{
								double t_min_val = INF, t_fa_manner = -1, t_fa_pos = -1;
								for (int i = tid; i < l; i += config.thread_count)
									for (int _m = 0; _m < DPConfig::kMannerCount; _m ++) {
										if (Clash(Index(crane_ord[c - 1], _m, i, l - 1), cur_index))
											continue ;
										double tmp = std::max(Dp[c - 1][_m][i][l - 1], ti);
										if (tmp < t_min_val)
											t_min_val = tmp, t_fa_manner = _m, t_fa_pos = i;
									}
								std::lock_guard<std::mutex> lock(mutex);
								if (t_min_val < min_val)
									min_val = t_min_val, fa_manner = t_fa_manner, fa_pos = t_fa_pos;
							});
						for (std::thread &thread : threads)
							thread.join();
					}
				}
		}
	// TODO : Construct Woring Plan
	CraneWorkingPlan plan;
	return plan;
}
