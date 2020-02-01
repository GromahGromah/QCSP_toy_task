#include "vessel.h"

#include <cassert>
#include <algorithm>

bool Crane::operator < (const Crane &rhs) const {
	return init_pos < rhs.init_pos;
}

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
