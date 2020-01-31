#ifndef VESSEL_H
#define VESSEL_H

#include <vector>
	
struct Crane {
	double teu_per_h;
	double bg_time;
	double ed_time;
	int bay_pos;
	bool operator < (const Crane &rhs) const {
		return bay_pos < rhs.bay_pos;
	}
};

struct Vessel {
	int bays;
	std::vector<int> teus_per_bay;
	std::vector<Crane> cranes;
};

#endif /* VESSEL_H */
