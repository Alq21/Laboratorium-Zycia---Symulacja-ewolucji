#ifndef LINEAGE_H
#define LINEAGE_H

class Organism;

namespace Lineage {

int allocateId();
bool areDirectKin(const Organism* a, const Organism* b);

} // namespace Lineage

#endif // LINEAGE_H
