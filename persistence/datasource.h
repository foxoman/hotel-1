#ifndef PERSISTENCE_DATASOURCE_H
#define PERSISTENCE_DATASOURCE_H

#include "persistence/op/operations.h"
#include "persistence/op/results.h"
#include "persistence/resultintegrator.h"
#include "persistence/sqlite/sqlitebackend.h"

#include "hotel/planning.h"

#include "boost/signals2.hpp"

#include <memory>
#include <mutex>
#include <queue>
#include <set>

namespace persistence
{

  /**
   * @brief The DataSource class handles all writing and reading access to the data.
   */
  class DataSource
  {
  public:
    DataSource(const std::string& databaseFile);
    ~DataSource();

    hotel::HotelCollection& hotels();
    const hotel::HotelCollection& hotels() const;
    hotel::PlanningBoard& planning();
    const hotel::PlanningBoard& planning() const;

    /**
     * @brief queueOperation queues a given operation to perform on the data
     * @param operation The operation to perform
     *
     * @note The operation might be performed immediately or queued for later execution
     */
    op::Task<op::OperationResults> queueOperation(op::Operation operation);

    /**
     * @brief queueOperations queues multiple operations
     * The operations are executed together under a transaction if possible.
     * @param operations List of operations to perform.
     */
    op::Task<op::OperationResults> queueOperations(op::Operations operations);

    void processIntegrationQueue();

    //! Returns the number of operations that the backend has yet to process
    size_t pendingOperationsCount() const;

    /**
     * @brief taskCompletedSignal returns the signal which is triggered when new results are waiting to be integrated
     * @note The signal is not called on the main thread, but on the backend worker thread
     */
    boost::signals2::signal<void(int)>& taskCompletedSignal() { return _backend.taskCompletedSignal(); }

  private:
    // Backing store and result integrator
    persistence::sqlite::SqliteBackend _backend;
    persistence::ResultIntegrator _resultIntegrator;

    int _nextOperationId;
  };

} // namespace persistence

#endif // PERSISTENCE_DATASOURCE_H
