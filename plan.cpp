#include "plan.h"

#include <cmath>
#include <cstdio>
#include <algorithm>

Action Action::Work(int pos, int teu, double bgt, double edt) {
	Action ret;
	ret.type = ActionType::Working;
	ret.w_detail.bay_pos = pos;
	ret.w_detail.teus = teu;
	ret.bg_time = bgt;
	ret.ed_time = edt;
	return ret;
}

Action Action::Move(int stb, int edb, double bgt, double edt) {
	Action ret;
	ret.type = ActionType::Moving;
	ret.m_detail.st_bay = stb;
	ret.m_detail.ed_bay = edb;
	ret.bg_time = bgt;
	ret.ed_time = edt;
	return ret;
}

int Action::CraneMinDist(const Action &lhs, const Action &rhs) {
	static constexpr double eps = 1e-6;
	static constexpr int INF = 0x3f3f3f3f;
	double inter_l = std::max(lhs.bg_time, rhs.bg_time);
	double inter_r = std::max(rhs.ed_time, rhs.ed_time);
	if (inter_l + eps > inter_r)
		return INF;
	std::vector<int> lhs_bays = (lhs.type == ActionType::Working)
		? std::vector<int>({lhs.w_detail.bay_pos})
		: std::vector<int>({lhs.m_detail.st_bay, lhs.m_detail.ed_bay});
	std::vector<int> rhs_bays = (rhs.type == ActionType::Working)
		? std::vector<int>({rhs.w_detail.bay_pos})
		: std::vector<int>({rhs.m_detail.st_bay, rhs.m_detail.ed_bay});
	int ret = INF;
	for (int l_bay : lhs_bays)
		for (int r_bay : rhs_bays)
			ret = std::min(ret, abs(l_bay - r_bay));
	return ret;
}

void Action::Print(FILE * fout) const {
	fprintf(fout, "%.3fh - %.3fh : ", bg_time, ed_time);
	if (type == ActionType::Working) {
		fprintf(fout, "Working at Bay-Pos %d with %d TEU(s)", w_detail.bay_pos + 1, w_detail.teus);
	}
	else {
		fprintf(fout, "Moving from Bay-Pos %d to Bay-Pos %d", m_detail.st_bay + 1, m_detail.ed_bay + 1);
	}
	fprintf(fout, "\n");
}

Action &WorkingSequence::operator [](int id) {
	return actions[id];
}

const Action &WorkingSequence::operator [](int id) const {
	return actions[id];
}

WorkingSequence WorkingSequence::GetMergeMovingActions() const {
	WorkingSequence ret;
	for (int l = 0, r; l < int(actions.size()); l = r) {
		for (r = l; r < int(actions.size()) && actions[r].type == Action::ActionType::Moving; r ++) ;
		if (l == r)
			ret.actions.push_back(actions[r ++]);
		else {
			int st_bay = actions[l].m_detail.st_bay;
			int ed_bay = actions[r - 1].m_detail.ed_bay;
			double bg_time = actions[l].bg_time;
			double ed_time = actions[r - 1].ed_time;
			ret.actions.push_back(Action::Move(st_bay, ed_bay, bg_time, ed_time));
		}
	}
	return ret;
}

WorkingSequence WorkingSequence::GetRemoveMovingActions() const {
	WorkingSequence ret;
	for (const Action &action : actions)
		if (action.type == Action::ActionType::Working)
			ret.actions.push_back(action);
	return ret;
}

void WorkingSequence::Print(FILE * fout) const {
	for (const Action &action : actions)
		action.Print(fout);
}

WorkingSequence &CraneWorkingPlan::operator [](int id) {
	return crane_seqs[id];
}

const WorkingSequence &CraneWorkingPlan::operator [](int id) const {
	return crane_seqs[id];
}

CraneWorkingPlan CraneWorkingPlan::GetMergeMovingActions() const {
	CraneWorkingPlan ret;
	for (const WorkingSequence &seq : crane_seqs)
		ret.crane_seqs.push_back(seq.GetMergeMovingActions());
	return ret;
}

CraneWorkingPlan CraneWorkingPlan::GetRemoveMovingActions() const {
	CraneWorkingPlan ret;
	for (const WorkingSequence &seq : crane_seqs)
		ret.crane_seqs.push_back(seq.GetRemoveMovingActions());
	return ret;
}

void CraneWorkingPlan::Print(FILE * fout) const {
	for (int c = 0; c < int(crane_seqs.size()); c ++) {
		fprintf(fout, "----- Crane %d -----\n", c + 1);
		crane_seqs[c].Print(fout);
	}
}

BayAction::BayAction(int crane, int teu, double bg_time, double ed_time)
	: crane(crane), teu(teu), bg_time(bg_time), ed_time(ed_time) {
}

void BayAction::Load(FILE * fin) {
	fscanf(fin, "%d%d%lf%lf", &crane, &teu, &bg_time, &ed_time);
}

void BayAction::Save(FILE * fout) const {
	fprintf(fout, "%d %d %.3f %.3f\n", crane, teu, bg_time, ed_time);
}

BayAction &BaySequence::operator [](int id) {
	return actions[id];
}

const BayAction &BaySequence::operator [](int id) const {
	return actions[id];
}

void BaySequence::Load(FILE * fin) {
	int cnt;
	scanf("%d", &cnt);
	actions.resize(cnt);
	for (BayAction &action : actions)
		action.Load(fin);
}

void BaySequence::Save(FILE * fout) const {
	fprintf(fout, "%d\n", int(actions.size()));
	for (const BayAction &action : actions)
		action.Save(fout);
}

BaySequence &BayPlan::operator [](int id) {
	return bay_seqs[id];
}

const BaySequence &BayPlan::operator [](int id) const {
	return bay_seqs[id];	
}

BayPlan BayPlan::ExtractBayPlan(const Vessel &vessel, const CraneWorkingPlan &crane_plan) {
	BayPlan ret;
	ret.bay_seqs.resize(vessel.bays);
	CraneWorkingPlan w_plan = crane_plan.GetRemoveMovingActions();
	for (int i = 0; i < int(w_plan.crane_seqs.size()); i ++) {
		const WorkingSequence &crane_seq = w_plan[i];
		for (const Action &crane_action : crane_seq.actions) {
			int bay_pos = crane_action.w_detail.bay_pos;
			int teu = crane_action.w_detail.teus;
			double bg_time = crane_action.bg_time;
			double ed_time = crane_action.ed_time;
			ret.bay_seqs[bay_pos].actions.emplace_back(i, teu, bg_time, ed_time);
		}
	}
	for (BaySequence &bay_seq : ret.bay_seqs)
		std::sort(bay_seq.actions.begin(), bay_seq.actions.end(), [&](const BayAction &lhs, const BayAction &rhs) {
			return lhs.bg_time < rhs.bg_time;
		});
	return ret;
}

void BayPlan::Load(FILE * fin) {
	int bays;
	scanf("%d", &bays);
	bay_seqs.resize(bays);
	for (BaySequence &seq : bay_seqs)
		seq.Load(fin);
}

void BayPlan::Save(FILE * fout) const {
	fprintf(fout, "%d\n", int(bay_seqs.size()));
	for (const BaySequence &seq : bay_seqs)
		seq.Save(fout);
}

