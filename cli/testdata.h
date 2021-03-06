#ifndef CLI_TESTDATA_H
#define CLI_TESTDATA_H

#include "hotel/hotel.h"
#include "hotel/hotelcollection.h"
#include "hotel/planning.h"

#include <memory>
#include <random>
#include <vector>

namespace cli
{

  std::vector<std::unique_ptr<hotel::Hotel>> createTestHotels(std::mt19937& rng);
  std::unique_ptr<hotel::PlanningBoard> createTestPlanning(std::mt19937& rng, hotel::HotelCollection& hotels);

} // namespaec cli

#endif // CLI_TESTDATA_H
