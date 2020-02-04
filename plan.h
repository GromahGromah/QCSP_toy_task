#ifndef PLAN_H
#define PLAN_H

#include <cstdio>
#include <vector>

#include "vessel.h"

struct Action {
	struct WorkingDetail {
		int bay_pos;
		int teus;
	};
	struct MovingDetail {
		int st_bay;
		int ed_bay;
	};
	enum ActionType {
		Working,
		Moving
	};
	
	static Action Work(int pos, int teu, double bgt, double edt);
	static Action Move(int stb, int edb, double bgt, double edt);
	static int CraneMinDist(const Action &lhs, const Action &rhs);
	ActionType type;
	WorkingDetail w_detail;
	MovingDetail m_detail;
	double bg_time;
	double ed_time;
	void Print(FILE * fout = stderr) const;
};

struct WorkingSequence {
	std::vector<Action> actions;
	Action &operator [](int id);
	const Action &operator [](int id) const;
	WorkingSequence GetMergeMovingActions() const;
	WorkingSequence GetRemoveMovingActions() const;
	void Print(FILE * fout = stderr) const;
};

struct CraneWorkingPlan {
	std::vector<WorkingSequence> crane_seqs;
	WorkingSequence &operator [](int id);
	const WorkingSequence &operator [](int id) const;
	CraneWorkingPlan GetMergeMovingActions() const;
	CraneWorkingPlan GetRemoveMovingActions() const;
	void Print(FILE * fout = stderr) const;
};

struct BayAction {
	int crane;
	int teu;
	double bg_time;
	double ed_time;
	BayAction() = default;
	BayAction(int crane, int teu, double bg_time, double ed_time);
	void Load(FILE * fin);
	void Save(FILE * fout) const;
};

struct BaySequence {
	std::vector<BayAction> actions;
	BayAction &operator [](int id);
	const BayAction &operator [](int id) const;
	void Load(FILE * fin);
	void Save(FILE * fout) const;
};

struct BayPlan {
	std::vector<BaySequence> bay_seqs;
	BaySequence &operator [](int id);
	const BaySequence &operator [](int id) const;
	static BayPlan ExtractBayPlan(const Vessel &vessel, const CraneWorkingPlan &crane_plan);
	void Load(FILE * fin);
	void Save(FILE * fout) const;
};

#endif /* PLAN_H */
