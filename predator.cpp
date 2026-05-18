#include "predator.h"
#include "producer.h"
#include "omnivore.h"
#include "actioneconomy.h"
#include "movementplanning.h"
#include "world.h"
#include <algorithm>
#include <cstdlib>
#include <limits>

namespace {
constexpr int TypeMutationChancePercent = 5;
constexpr double ChildEnergyFraction = 0.6;
constexpr double PredatorMetabolismPerTick = 2.0;
constexpr double PredatorReproduceThreshold = 80.0;
constexpr double PredatorReproductionEnergyScale = 0.72;
constexpr int PredatorReproductionApDiscount = 2;
constexpr int LeapCooldownTicks = 5;

int predatorReproductionApCost(const Predator& predator)
{
    return std::max(2, predator.reproductionApCost() - PredatorReproductionApDiscount);
}

double predatorReproductionEnergyCost(const Predator& predator)
{
    return predator.reproductionEnergyCost() * PredatorReproductionEnergyScale;
}
}

Predator::Predator(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
                   int vision, TemperaturePreference tempPreference)
    : Organism(pos, col, startEnergy, maxEn, size, speed, maxAP, gen, tempPreference), visionRange(vision) {
}

int Predator::getVision() const
{
    return std::max(visionRange, MovementPlanning::minimumPredatorVisionRadius());
}

Producer* Predator::findBestPreyProducer(World* world) const {
    if (!world) return nullptr;

    Producer* bestPrey = nullptr;
    const int vision = getVision();
    double bestScore = std::numeric_limits<double>::max();

    for (const auto& org : world->getOrganisms()) {
        if (!org || !org->getIsAlive() || org.get() == this) continue;

        Producer* prey = dynamic_cast<Producer*>(org.get());
        if (!prey) continue; // Ignoruje wszystożerców i innych drapieżników

        const Position preyPos = prey->getPosition();
        const double dx = preyPos.x - position.x;
        const double dy = preyPos.y - position.y;
        const double distSq = (dx * dx) + (dy * dy);

        if (distSq <= (vision * vision)) {
            // Punktacja: mniejszy wynik = atrakcyjniejsza ofiara
            // Dystans jest kluczowy. Szybka ofiara to trudniejszy cel (+). Kaloryczna ofiara to lepszy cel (-).
            double score = distSq + (prey->getSpeed() * 3.0) - (prey->getEnergy() * 0.05);

            if (score < bestScore) {
                bestScore = score;
                bestPrey = prey;
            }
        }
    }

    return bestPrey;
}

bool Predator::canLeap() const {
    return leapCooldown <= 0;
}

void Predator::onTick(World* world) {
    if (!isAlive) return;

    if (leapCooldown > 0) leapCooldown--;
    isLeapingNow = false;

    setEnergy(energy - PredatorMetabolismPerTick * climateMetabolismFactor(world));
    refillActionPoints();

    if (energy <= 0) die();
}

void Predator::planMove(World* world) {
    leapPlanned = false;

    if (!isAlive || actionPoints < movementApCost()) {
        plannedPosition = position;
        return;
    }

    Producer* prey = findBestPreyProducer(world);
    if (prey) {
        int dx = prey->getPosition().x - position.x;
        int dy = prey->getPosition().y - position.y;
        double distSq = dx * dx + dy * dy;

        // Jeśli może skoczyć i ofiara jest w odpowiedniej odległości (dystans od 2 do 5 pól -> distSq > 2.0 do 25.0)
        if (canLeap() && distSq > 2.0 && distSq <= 25.0) {

            // 1. Przewidujemy, gdzie ofiara ucieknie w tej turze
            int escapeDx = 0;
            int escapeDy = 0;
            MovementPlanning::predictProducerEscapeStep(prey, world, escapeDx, escapeDy);

            Position futurePreyPos{prey->getPosition().x + escapeDx, prey->getPosition().y + escapeDy};

            // 2. Celujemy w jej PRZYSZŁĄ pozycję
            int targetDx = futurePreyPos.x - position.x;
            int targetDy = futurePreyPos.y - position.y;

            // 3. Skok o max 3 pola by odciąć drogę
            int jumpX = (targetDx > 0) ? std::min(3, targetDx) : ((targetDx < 0) ? std::max(-3, targetDx) : 0);
            int jumpY = (targetDy > 0) ? std::min(3, targetDy) : ((targetDy < 0) ? std::max(-3, targetDy) : 0);

            Position landing{position.x + jumpX, position.y + jumpY};

            // BEZPIECZNY SKOK: Weryfikujemy czy pole jest wewnątrz mapy
            if (landing.x >= 0 && landing.x < world->getWidth() &&
                landing.y >= 0 && landing.y < world->getHeight()) {

                Organism* occupant = world->getOrganismAt(landing);
                // Lądujemy, jeśli pole jest puste LUB jeśli wylądujemy dokładnie na naszej ofierze
                if (occupant == nullptr || occupant == prey) {
                    plannedPosition = landing;
                    leapPlanned = true;
                    return;
                }
            }
        }

        // Standardowe ściganie
        plannedPosition = MovementPlanning::choosePredatorPursuitPosition(this, prey, world);

        if (plannedPosition.x < 0) { // Zabezpieczenie na wypadek braku drogi
            plannedPosition = position;
        }
        return;
    }

    // Swobodne wędrowanie, jeśli w zasięgu wzroku nie ma ofiar
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    Position randomTarget{position.x + dx, position.y + dy};

    if (randomTarget.x >= 0 && randomTarget.x < world->getWidth() &&
        randomTarget.y >= 0 && randomTarget.y < world->getHeight()) {
        plannedPosition = randomTarget;
    } else {
        plannedPosition = position;
    }
}

void Predator::executeMovement(World* world) {
    if (!isAlive) return;

    if (leapPlanned) {
        isLeapingNow = true;
        leapCooldown = LeapCooldownTicks; // Skok nakłada cooldown (np. 5 tur)
        leapPlanned = false;
    }

    Organism::executeMovement(world);
}

void Predator::onInteract(Entity* other) {
    if (!isAlive) return;

    Organism* prey = dynamic_cast<Organism*>(other);
    // Jeśli zderzyliśmy się z Producentem, próbujemy zapolować
    if (prey && prey->getIsAlive() && dynamic_cast<Producer*>(prey)) {
        attemptHunt(prey);
    }
}

bool Predator::attemptHunt(Organism* target) {
    if (!target || !target->getIsAlive() || !dynamic_cast<Producer*>(target)) return false;

    const int apCost = ActionEconomy::huntApCost(target->getSize());
    if (!trySpendActionPoints(apCost)) return false;

    // Pobiera energię proporcjonalnie do głodu drapieżnika
    const double spaceLeft = maxEnergy - energy;
    const double stolenEnergy = std::min(target->getEnergy(), spaceLeft);

    target->setEnergy(target->getEnergy() - stolenEnergy);
    setEnergy(energy + stolenEnergy);

    return isAlive;
}

bool Predator::canReproduce() const {
    if (!isAlive) return false;

    const int apCost = predatorReproductionApCost(*this);
    const double energyCost = predatorReproductionEnergyCost(*this);

    return actionPoints >= apCost && energy >= PredatorReproduceThreshold + energyCost;
}

std::unique_ptr<Organism> Predator::reproduce()
{
    if (!canReproduce()) return nullptr;

    const int apCost = predatorReproductionApCost(*this);
    const double energyCost = predatorReproductionEnergyCost(*this);
    if (!trySpendActionPoints(apCost)) return nullptr;

    energy -= energyCost;
    if (!isAlive) return nullptr;

    double childEnergy = energy * ChildEnergyFraction;
    energy -= childEnergy;

    Position childPos = position;
    double childMaxEn = maxEnergy;
    int childVision = visionRange;
    int childSpeed = speed;
    int childSize = size;
    Color childColor = color;

    if (rand() % 100 < 25) {
        childColor.r = 200 + (rand() % 56);
        childColor.g = rand() % 100;
        childColor.b = rand() % 100;

        childSize += (rand() % 3) - 1;
        if (childSize < 2) childSize = 2;
        if (childSize > 5) childSize = 5;

        childVision += (rand() % 5) - 2;
        if (childVision < 3) childVision = 3;
        if (childVision > 15) childVision = 15;
    }

    if (rand() % 100 < TypeMutationChancePercent) {
        int mutantSize = std::clamp(childSize, 1, 6);

        std::unique_ptr<Organism> mutant;
        if (rand() % 2 == 0) {
            mutant = std::make_unique<Producer>(
                childPos, Color{255, 220, 50}, childEnergy, childMaxEn,
                mutantSize, 1, maxActionPoints, generation + 1, temperaturePreference);
        } else {
            mutant = std::make_unique<Omnivore>(
                childPos, Color{200, 160, 220}, childEnergy, childMaxEn,
                mutantSize, childSpeed, maxActionPoints, generation + 1,
                childVision, temperaturePreference);
        }
        mutant->initLineageFromParent(this);
        return mutant;
    }

    // Mutacja preferencji temperaturowej (5% szansy)
    TemperaturePreference childTempPref = temperaturePreference;
    if (rand() % 100 < TypeMutationChancePercent) {
        int roll = rand() % 2;
        if (temperaturePreference == TemperaturePreference::Default)
            childTempPref = (roll == 0) ? TemperaturePreference::Cryophile : TemperaturePreference::Thermophile;
        else if (temperaturePreference == TemperaturePreference::Cryophile)
            childTempPref = (roll == 0) ? TemperaturePreference::Default : TemperaturePreference::Thermophile;
        else
            childTempPref = (roll == 0) ? TemperaturePreference::Default : TemperaturePreference::Cryophile;
    }

    auto child = std::make_unique<Predator>(
        childPos, childColor, childEnergy, childMaxEn, childSize, childSpeed,
        maxActionPoints, generation + 1, childVision, childTempPref
        );
    child->initLineageFromParent(this);
    return child;
}