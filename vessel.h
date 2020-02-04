#ifndef VESSEL_H
#define VESSEL_H

#include <string>
#include <vector>

struct Crane {
	double teu_per_h;
	int init_pos;
};

struct Vessel {
	int bays;
	int crane_min_dist;
	std::vector<double> move_time_between_bay;
	std::vector<int> teus_per_bay;
	std::vector<Crane> cranes;
	
	Vessel();
	Vessel(const std::string &filename);
	int GetTotalTEU() const;
	void Load(const std::string &filename);
	void Save(const std::string &filename) const;
};

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

#endif /* VESSEL_H */

