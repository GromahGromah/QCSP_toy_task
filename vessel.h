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

#endif /* VESSEL_H */

