/**
 * @file ParallelRaptor.h
 * @brief Runs batches of independent RAPTOR queries concurrently across worker threads.
 */
#ifndef RAPTOR_PARALLELRAPTOR_H
#define RAPTOR_PARALLELRAPTOR_H

#include "Raptor.h"

/**
 * @class ParallelRaptor
 * @brief Dispatches a batch of independent Query objects across multiple threads.
 *
 * Each worker thread owns its own Raptor instance (built from a copy of the same
 * timetable data), since Raptor keeps per-query state as instance members and is
 * not safe to share across threads. Results are returned in the same order as
 * the input queries.
 */
class ParallelRaptor {
public:
  ParallelRaptor(const std::unordered_map<std::string, Agency> &agencies,
                 const std::unordered_map<std::string, Calendar> &calendars,
                 const std::unordered_map<std::string, Stop> &stops,
                 const std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> &routes,
                 const std::unordered_map<std::string, Trip> &trips,
                 const std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> &stop_times);

  /**
   * @brief Runs all given queries, distributing them across worker threads.
   *
   * @param[in] queries The queries to run. Order of the result matches this order.
   * @param[in] num_workers Number of worker threads to use. Defaults (0) to
   *            std::thread::hardware_concurrency(), capped at queries.size().
   * @return One journey vector per query, in the same order as queries.
   */
  std::vector<std::vector<Journey>> findJourneys(const std::vector<Query> &queries, unsigned int num_workers = 0) const;

private:
  std::unordered_map<std::string, Agency> agencies_;
  std::unordered_map<std::string, Calendar> calendars_;
  std::unordered_map<std::string, Stop> stops_;
  std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> routes_;
  std::unordered_map<std::string, Trip> trips_;
  std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> stop_times_;
};

#endif //RAPTOR_PARALLELRAPTOR_H
