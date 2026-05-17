#include "lineage.h"
#include "organism.h"

namespace Lineage {

int allocateId()
{
    static int nextId = 1;
    return nextId++;
}

bool areDirectKin(const Organism* a, const Organism* b)
{
    if (!a || !b || a == b) {
        return true;
    }

    return a->getParentLineageId() == b->getLineageId() ||
           b->getParentLineageId() == a->getLineageId();
}

} // namespace Lineage
