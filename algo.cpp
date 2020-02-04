#include "algo.h"

#include <cmath>
#include <mutex>
#include <thread>
#include <cassert>
#include <algorithm>

#define T_STATIC thread_local static
#define INF 1e30

DPConfig::DPConfig()
	: DPConfig(kDefaultBatchSize) {
}

DPConfig::DPConfig(int batch_size)
	: batch_size(batch_size) {
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
	
	std::vector<int> batch_pos, batch_teu, prefix_batch_teu;
	for (int b = 0; b < int(batches_per_bay.size()); b ++)
		for (int i = 0, ret = vessel.teus_per_bay[b]; i < batches_per_bay[b]; i ++) {
			int teu = std::min(ret, batch_sz);
			batch_pos.push_back(b);
			batch_teu.push_back(teu);
			prefix_batch_teu.push_back((prefix_batch_teu.empty() ? 0 : prefix_batch_teu.back()) + teu);
			ret -= teu;
		}
		
	std::vector<double> prefix_move_time({0.0});
	for (double move_time : vessel.move_time_between_bay)
		prefix_move_time.push_back(prefix_move_time.back() + move_time);
	
	/* Solve */
	T_STATIC double Dp[DPConfig::kMaxCraneCount][DPConfig::kMannerCount][DPConfig::kMaxBatchCount][DPConfig::kMaxBatchCount];
	T_STATIC std::pair<int, int> Fa[DPConfig::kMaxCraneCount][DPConfig::kMannerCount][DPConfig::kMaxBatchCount][DPConfig::kMaxBatchCount];
	struct Index {
		int crane, manner, l, r;
		Index(int crane, int manner, int l, int r)
			: crane(crane), manner(manner), l(l), r(r) {
		}
	};
	auto MoveTime = [&](int lhs, int rhs) -> double {
		return std::abs(prefix_move_time[rhs] - prefix_move_time[lhs]);
	};
	auto Move = [&](int src_bay, int dst_bay, double &cur_time, WorkingSequence &seq) {
		int cur_bay = src_bay;
		while (cur_bay != dst_bay) {
			int nxt_bay = (cur_bay < dst_bay) ? (cur_bay + 1) : (cur_bay - 1);
			double cost = MoveTime(cur_bay, nxt_bay);
			seq.actions.push_back(Action::Move(cur_bay, nxt_bay, cur_time, cur_time + cost));
			cur_bay = nxt_bay;
			cur_time += cost;
		}
	};
	auto MoveWork = [&](int src_batch, int dst_batch, double teu_per_h, double &cur_time, WorkingSequence &seq) {
		int src_bay = batch_pos[src_batch], dst_bay = batch_pos[dst_batch];
		int cur_bay = src_bay;
		if (src_batch > dst_batch)
			std::swap(src_batch, dst_batch);
		while (true) {
			int r_batch = std::min(prefix_batches[cur_bay] - 1, dst_batch);
			int l_batch = std::max(cur_bay == 0 ? 0 : prefix_batches[cur_bay - 1], src_batch);
			if (l_batch <= r_batch) {
				int teu = prefix_batch_teu[r_batch] - prefix_batch_teu[l_batch] + batch_teu[l_batch];
				double cost = teu / teu_per_h;
				seq.actions.push_back(Action::Work(cur_bay, teu, cur_time, cur_time + cost));
				cur_time += cost;
			}
			
			if (cur_bay == dst_bay)
				break ;
			
			int nxt_bay = (cur_bay < dst_bay) ? (cur_bay + 1) : (cur_bay - 1);
			double cost = MoveTime(cur_bay, nxt_bay);
			seq.actions.push_back(Action::Move(cur_bay, nxt_bay, cur_time, cur_time + cost));
			cur_bay = nxt_bay;
			cur_time += cost;
		}
	};
	auto GetDetailedPlan = [&](const Index &index) -> WorkingSequence {
		/* Return Null Plan if Illegal */
		WorkingSequence ret;
		int l_bay = batch_pos[index.l], r_bay = batch_pos[index.r];
		int c_bay = vessel.cranes[index.crane].init_pos - 1;
		double teu_per_h = vessel.cranes[index.crane].teu_per_h;
		double cur_time = 0.0;
		if (index.manner == 0) {
			/* Manner 0 Steps(Always Legal):
			 * 1. Move to l_bay
			 * 2. MoveWork from l_bay to r_bay
			 */
			Move(c_bay, l_bay, cur_time, ret);
			MoveWork(index.l, index.r, teu_per_h, cur_time, ret);
		}
		else if (index.manner == 1) {
			/* Manner 1 Steps(Always Legal):
			 * 1. Move to r_bay
			 * 2. MoveWork from r_bay to l_bay
			 */
			Move(c_bay, r_bay, cur_time, ret);
			MoveWork(index.r, index.l, teu_per_h, cur_time, ret);
		}
		else if (index.manner == 2) {
			/* Manner 2 Steps(Legal when l_bay < c_bay < r_bay):
			 * 1. MoveWork from c_bay to l_bay
			 * 2. Move from l_bay to c_bay + 1
			 * 3. MoveWork from c_bay + 1 to r_bay
			 */
			if (l_bay < c_bay && c_bay < r_bay) {
				MoveWork(prefix_batches[c_bay] - 1, index.l, teu_per_h, cur_time, ret);
				Move(l_bay, batch_pos[prefix_batches[c_bay]], cur_time, ret);
				MoveWork(prefix_batches[c_bay], index.r, teu_per_h, cur_time, ret);
			}
		}
		else if (index.manner == 3) {
			/* Manner 3 Steps(Legal when l_bay < c_bay < r_bay):
			 * 1. MoveWork from c_bay to r_bay
			 * 2. Move from r_bay to c_bay - 1
			 * 3. MoveWork from c_bay - 1 to l_bay
			 */
			if (l_bay < c_bay && c_bay < r_bay) {
				MoveWork(prefix_batches[c_bay - 1], index.r, teu_per_h, cur_time, ret);
				Move(r_bay, batch_pos[prefix_batches[c_bay - 1] - 1], cur_time, ret);
				MoveWork(prefix_batches[c_bay - 1] - 1, index.l, teu_per_h, cur_time, ret);
			}
		}
		return ret;
	};
	auto Calc = [&](const Index &index) -> double {
		/* Return INF if Illegal */
		WorkingSequence seq = GetDetailedPlan(index);
		if (seq.actions.empty())
			return INF;
		return seq.actions.back().ed_time;
	};
	auto Clash = [&](const WorkingSequence &lhs_seq, const WorkingSequence &rhs_seq) -> bool {
		/* Return True if One Illegal */
		if (lhs_seq.actions.empty() || rhs_seq.actions.empty())
			return true;
		int safe_dist = vessel.crane_min_dist;
		for (int l = 0, r = 0; l < int(lhs_seq.actions.size()) && r < int(rhs_seq.actions.size()); ) {
			if (Action::CraneMinDist(lhs_seq[l], rhs_seq[r]) < safe_dist)
				return true;
			if (lhs_seq[l].ed_time < rhs_seq[r].ed_time)
				l ++;
			else r ++;
		}
		return false;
	};
	
	fprintf(stderr, "Now Solving, cranes = %d, manners = %d, batches = %d\n", cranes, manners, batches);
	for (int batch : batches_per_bay)
		fprintf(stderr, "%d ", batch);
	fprintf(stderr, "\n");
	for (int c = 0; c < cranes; c ++)
		for (int m = 0; m < manners; m ++)
			for (int l = 0; l < batches; l ++)
				for (int r = 0; r < batches; r ++)
					Dp[c][m][l][r] = INF;
	int tot_tasks = cranes * manners, finished = 0;
	for (int c = 0; c < cranes; c ++)
		for (int m = 0; m < manners; m ++) {
			int l_st = c, l_ed = (c == 0 ? 0 : batches - 1);
			for (int l = l_st; l <= l_ed; l ++) {
				for (int r = l; r < batches; r ++) {
					Index cur_index = Index(crane_ord[c], m, l, r);
					WorkingSequence cur_seq = GetDetailedPlan(cur_index);
					if (cur_seq.actions.empty())
						continue ;
					double ti = Calc(cur_index);
					if (c == 0)
						Dp[c][m][l][r] = ti;
					else {
						double &min_val = Dp[c][m][l][r];
						int &fa_manner = Fa[c][m][l][r].first;
						int &fa_pos = Fa[c][m][l][r].second;
						min_val = INF, fa_manner = -1, fa_pos = -1;
						
						for (int _m = 0; _m < DPConfig::kMannerCount; _m ++)
							for (int i = l - 1; i >= 0; i --) {
								WorkingSequence pre_seq = GetDetailedPlan(Index(crane_ord[c - 1], _m, i, l - 1));
								if (pre_seq.actions.empty())
									continue ;
								if (pre_seq.actions.back().ed_time > min_val)
									break ;
								if (Clash(pre_seq, cur_seq))
									continue ;
								double tmp = std::max(Dp[c - 1][_m][i][l - 1], ti);
								if (tmp < min_val)
									min_val = tmp, fa_manner = _m, fa_pos = i;
							}
					}
				}
			}
			fprintf(stderr, "Process : %d/%d\n", ++ finished, tot_tasks);
		}
	double best_time = INF;
	int crane = -1, manner = -1, l_pos = -1;
	for (int c = 0; c < cranes; c ++)
		for (int m = 0; m < manners; m ++) {
			int l_st = c, l_ed = (c == 0 ? 0 : batches - 1);
			for (int l = l_st; l <= l_ed; l ++)
				if (Dp[c][m][l][batches - 1] < best_time)
					best_time = Dp[c][m][l][batches - 1], crane = c, manner = m, l_pos = l;
		}
	fprintf(stderr, "Best Time = %.3fh\n", best_time);
	CraneWorkingPlan plan;
	plan.crane_seqs.resize(vessel.cranes.size());
	for (int c = crane, m = manner, l = l_pos, r = batches - 1; c >= 0; c --) {
		fprintf(stderr, "Crane %d : Do Batches [%d, %d] with Manner %d\n", crane_ord[c], l, r, m);
		plan[crane_ord[c]] = GetDetailedPlan(Index(crane_ord[c], m, l, r));
		int fa_m = Fa[c][m][l][r].first, fa_l = Fa[c][m][l][r].second;
		m = fa_m, r = l - 1, l = fa_l;
	}
	return plan;
}
