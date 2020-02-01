#ifndef VESSEL_H
#define VESSEL_H

#include <vector>
	
struct Crane {
	double teu_per_h;
	int init_pos;
	bool operator < (const Crane &rhs) const;
};

struct Vessel {
	int bays;
	int crane_min_dist;
	std::vector<double> move_time_between_bay;
	std::vector<int> teus_per_bay;
	std::vector<Crane> cranes;
};

#endif /* VESSEL_H */

