/**
 * @file parallelRaptor.cpp
 * @brief Unit tests for ParallelRaptor, checking parity with sequential Raptor queries.
 */

#include "gtest/gtest.h"
#include "./src/ParallelRaptor.h"

static void loadData(
        const std::vector<std::string> &inputDirectories,
        std::unordered_map<std::string, Agency> &agencies,
        std::unordered_map<std::string, Calendar> &calendars,
        std::unordered_map<std::string, Trip> &trips,
        std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> &routes,
        std::unordered_map<std::string, Stop> &stops,
        std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> &stop_times) {

  for (const auto &dir: inputDirectories) {
    Parser parser(dir);

    auto dirAgencies = parser.getAgencies();
    agencies.insert(dirAgencies.begin(), dirAgencies.end());

    auto dirCalendars = parser.getCalendars();
    calendars.insert(dirCalendars.begin(), dirCalendars.end());

    auto dirTrips = parser.getTrips();
    trips.insert(dirTrips.begin(), dirTrips.end());

    auto dirRoutes = parser.getRoutes();
    routes.insert(dirRoutes.begin(), dirRoutes.end());

    auto dirStops = parser.getStops();
    stops.insert(dirStops.begin(), dirStops.end());

    auto dirStopTimes = parser.getStopTimes();
    stop_times.insert(dirStopTimes.begin(), dirStopTimes.end());
  }
}

class ParallelRaptorTests : public ::testing::Test {
protected:
  static std::unordered_map<std::string, Agency> agencies;
  static std::unordered_map<std::string, Calendar> calendars;
  static std::unordered_map<std::string, Trip> trips;
  static std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> routes;
  static std::unordered_map<std::string, Stop> stops;
  static std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> stop_times;

  static void SetUpTestSuite() {
    std::vector<std::string> inputDirectories = {std::string(DATASET_PATH) + "/Porto/stcp/GTFS/",
                                                 std::string(DATASET_PATH) + "/Porto/metro/GTFS/"};

    loadData(inputDirectories, agencies, calendars, trips, routes, stops, stop_times);
  }
};

std::unordered_map<std::string, Agency> ParallelRaptorTests::agencies;
std::unordered_map<std::string, Calendar> ParallelRaptorTests::calendars;
std::unordered_map<std::string, Trip> ParallelRaptorTests::trips;
std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> ParallelRaptorTests::routes;
std::unordered_map<std::string, Stop> ParallelRaptorTests::stops;
std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> ParallelRaptorTests::stop_times;

/**
 * @test MatchesSequentialResults
 * @brief Checks that batched, multithreaded queries return the same journeys
 * (in the same order) as running each query sequentially on its own Raptor instance.
 */
TEST_F(ParallelRaptorTests, MatchesSequentialResults) {
  std::vector<Query> queries = {
          {"5777", "5776", {2024, 10, 15}, {22, 30, 0}},
          {"5777", "OTH2", {2024, 10, 15}, {11, 30, 0}},
          {"ELVG1", "5733", {2024, 11, 20}, {14, 0, 0}},
          {"MAIA3", "PARR3", {2024, 10, 15}, {5, 55, 0}},
          {"SAL2", "IPO5", {2024, 12, 11}, {14, 0, 0}},
          {"5726", "5739", {2024, 10, 15}, {6, 44, 0}},
          {"5753", "5782", {2024, 10, 15}, {19, 44, 0}},
          {"5717", "PREC2", {2024, 10, 20}, {20, 20, 0}},
  };

  ParallelRaptor parallel_raptor(agencies, calendars, stops, routes, trips, stop_times);
  std::vector<std::vector<Journey>> parallel_results = parallel_raptor.findJourneys(queries);

  ASSERT_EQ(parallel_results.size(), queries.size());

  for (size_t i = 0; i < queries.size(); i++) {
    Raptor raptor(agencies, calendars, stops, routes, trips, stop_times);
    raptor.setQuery(queries[i]);
    std::vector<Journey> sequential_result = raptor.findJourneys();

    ASSERT_EQ(parallel_results[i].size(), sequential_result.size());
    for (size_t j = 0; j < sequential_result.size(); j++) {
      EXPECT_EQ(parallel_results[i][j].departure_secs, sequential_result[j].departure_secs);
      EXPECT_EQ(parallel_results[i][j].arrival_secs, sequential_result[j].arrival_secs);
      EXPECT_EQ(parallel_results[i][j].duration, sequential_result[j].duration);
      EXPECT_EQ(parallel_results[i][j].steps.size(), sequential_result[j].steps.size());
    }
  }
}

/**
 * @test HandlesSingleWorker
 * @brief Forcing a single worker thread should still process every query.
 */
TEST_F(ParallelRaptorTests, HandlesSingleWorker) {
  std::vector<Query> queries = {
          {"5777", "5776", {2024, 10, 15}, {22, 30, 0}},
          {"5726", "5739", {2024, 10, 15}, {6, 44, 0}},
  };

  ParallelRaptor parallel_raptor(agencies, calendars, stops, routes, trips, stop_times);
  std::vector<std::vector<Journey>> results = parallel_raptor.findJourneys(queries, 1);

  ASSERT_EQ(results.size(), queries.size());
  for (const auto &journeys: results) EXPECT_FALSE(journeys.empty());
}

/**
 * @test HandlesEmptyBatch
 * @brief An empty query batch should return an empty result without spawning threads.
 */
TEST_F(ParallelRaptorTests, HandlesEmptyBatch) {
  ParallelRaptor parallel_raptor(agencies, calendars, stops, routes, trips, stop_times);
  std::vector<std::vector<Journey>> results = parallel_raptor.findJourneys({});

  EXPECT_TRUE(results.empty());
}
