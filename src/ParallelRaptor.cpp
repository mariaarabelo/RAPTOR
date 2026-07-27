/**
 * @file ParallelRaptor.cpp
 * @brief ParallelRaptor class implementation.
 */
#include "ParallelRaptor.h"
#include <thread>
#include <algorithm>

ParallelRaptor::ParallelRaptor(const std::unordered_map<std::string, Agency> &agencies,
                               const std::unordered_map<std::string, Calendar> &calendars,
                               const std::unordered_map<std::string, Stop> &stops,
                               const std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> &routes,
                               const std::unordered_map<std::string, Trip> &trips,
                               const std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> &stop_times)
        : agencies_(agencies), calendars_(calendars), stops_(stops), routes_(routes), trips_(trips),
          stop_times_(stop_times) {}

std::vector<std::vector<Journey>> ParallelRaptor::findJourneys(const std::vector<Query> &queries,
                                                                 unsigned int num_workers) const {
  std::vector<std::vector<Journey>> results(queries.size());
  if (queries.empty()) return results;

  if (num_workers == 0) num_workers = std::thread::hardware_concurrency();
  num_workers = std::max(1u, std::min(num_workers, static_cast<unsigned int>(queries.size())));

  std::vector<std::thread> workers;
  workers.reserve(num_workers);

  // Each worker builds its own Raptor from a copy of the timetable data, since
  // Raptor keeps per-query state (arrivals_, marked_stops, k, query_, ...) as
  // instance members and mutates trips_ on every findJourneys() call.
  for (unsigned int w = 0; w < num_workers; ++w) {
    workers.emplace_back([&, w]() {
      Raptor raptor(agencies_, calendars_, stops_, routes_, trips_, stop_times_);
      for (size_t i = w; i < queries.size(); i += num_workers) {
        raptor.setQuery(queries[i]);
        results[i] = raptor.findJourneys();
      }
    });
  }

  for (auto &worker : workers) worker.join();

  return results;
}
