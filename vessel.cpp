#include "vessel.h"

#include <cassert>
#include <algorithm>

Vessel::Vessel() {
}

Vessel::Vessel(const std::string &filename) {
	Load(filename);
}

int Vessel::GetTotalTEU() const {
	int ret = 0;
	for (int teu : teus_per_bay)
		ret += teu;
	return ret;
}

void Vessel::Load(const std::string &filename) {
	FILE *fin = fopen(filename.c_str(), "r");
	assert(fin != NULL);
	fscanf(fin, "%d%d", &bays, &crane_min_dist);
	move_time_between_bay.resize(bays - 1);
	for (double &ti : move_time_between_bay)
		fscanf(fin, "%lf", &ti);
	teus_per_bay.resize(bays);
	for (int &teu : teus_per_bay)
		fscanf(fin, "%d", &teu);
	int number_of_bays;
	fscanf(fin, "%d", &number_of_bays);
	cranes.resize(number_of_bays);
	for (Crane &crane : cranes)
		fscanf(fin, "%lf%d", &crane.teu_per_h, &crane.init_pos);
	fclose(fin);
}

void Vessel::Save(const std::string &filename) const {
	FILE *fout = fopen(filename.c_str(), "w");
	assert(fout != NULL);
	fprintf(fout, "%d %d\n", bays, crane_min_dist);
	for (int i = 0; i < bays - 1; i ++)
		fprintf(fout, "%.3f%c", move_time_between_bay[i], " \n"[i == bays - 2]);
	for (int i = 0; i < bays; i ++)
		fprintf(fout, "%d%c", teus_per_bay[i], " \n"[i == bays - 1]);
	fprintf(fout, "%d\n", int(cranes.size()));
	for (const Crane &crane : cranes)
		fprintf(fout, "%.3f %d\n", crane.teu_per_h, crane.init_pos);
	fclose(fout);
}

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

Action &WorkingSequence::operator [](int id) {
	return actions[id];
}

void WorkingSequence::MergeMovingActions() {
	// TODO : Complete Code
}

void RemoveMovingActions() {
	// TODO : Complete Code
}

WorkingSequence &CraneWorkingPlan::operator [](int id){
	return crane_seqs[id];
}
